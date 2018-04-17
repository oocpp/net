#include "TcpClient.h"
#include"TcpConnection.h"
#include "Log.h"
#include "Socket.h"

namespace net
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    std::atomic<uint64_t> TcpClient::id{0};

    TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &nameArg)
            : _loop(loop)
              , _connector(new Connector(loop, serverAddr))
              , _peer_addr(serverAddr)
              , _name(nameArg)
              , _connection(nullptr)
              , _retry(true)
              , _status(Disconnected)
    {
        LOG_TRACE;
        _connector->set_new_connection_cb([this](int fd, const InetAddress &addr){on_new_connection(fd,addr);});
        _connector->set_connect_error_cb([this](int fd, const InetAddress &addr) { handle_connect_failed(fd, addr); });
    }

    TcpClient::~TcpClient()noexcept
    {
        assert(_status == Disconnected);
    }

    void TcpClient::connect()
    {
        LOG_TRACE;
        assert(_status == Disconnected);

        Status t = Disconnected;
        if (_status.compare_exchange_strong(t, Connecting)) {
            _connector->start();
        }
    }

    void TcpClient::disconnect()
    {
        LOG_TRACE;
        if (_status == Disconnected)
            return;

        if (_status.exchange(Disconnected) == Connecting)
            _connector->cancel();
        else
            _loop->run_in_loop([this] { disconnect_in_loop(); });
    }

    void TcpClient::disconnect_in_loop()
    {
        if (_connection) {
            _connection->close(false);
            _connection.reset();
        }
    }

    void TcpClient::on_new_connection(int fd, const InetAddress &addr)
    {
        Status t = Connecting;
        if (_status.compare_exchange_strong(t, Connected)) {

            _connection.reset(new TcpConnection(++id, _loop, fd, addr, _peer_addr));

            _connection->set_message_cb(_message_cb);
            _connection->set_connection_cb(_connecting_cb);
            _connection->set_write_complete_cb(_write_complete_cb);

            _connection->set_close_cb([this](const TCPConnPtr &conn){on_remove_connection(conn);});

            auto temp=_connection;
            _loop->run_in_loop([temp]{temp->attach_to_loop();});

            LOG_TRACE << "Client new conn";
        }
        else{
            Socket::close(fd);
        }
    }

    void TcpClient::on_remove_connection(const TCPConnPtr &conn)
    {
        _connection.reset();

        Status t = Connected;
        if (_retry && _status.compare_exchange_strong(t, Connecting)) {
            LOG_INFO << "retry";
            _connector->restart();
        }
        else {
            _status = Disconnected;
        }
    }

    void TcpClient::set_retry(bool t)
    {
        _retry = t;
    }

    EventLoop *TcpClient::get_loop()
    {
        return _loop;
    }

    const std::string &TcpClient::get_name()
    {
        return _name;
    }

    Any &TcpClient::get_context()
    {
        return _context;
    }

    void TcpClient::set_context(Any &&a)
    {
        _context = std::move(a);
    }

    void TcpClient::set_context(const Any &a)
    {
        _context = a;
    }

    void TcpClient::set_connection_cb(const ConnectingCallback &cb)
    {
        assert(_status == Disconnected);
        _connecting_cb = cb;
    }

    void TcpClient::set_message_cb(const MessageCallback &cb)
    {
        assert(_status == Disconnected);
        _message_cb = cb;
    }

    void TcpClient::set_write_complete_cb(const WriteCompleteCallback &cb)
    {
        assert(_status == Disconnected);
        _write_complete_cb = cb;
    }

    void TcpClient::set_connection_cb(ConnectingCallback &&cb) noexcept
    {
        assert(_status == Disconnected);
        _connecting_cb = std::move(cb);
    }

    void TcpClient::set_message_cb(MessageCallback &&cb)noexcept
    {
        assert(_status == Disconnected);
        _message_cb = std::move(cb);
    }

    void TcpClient::set_write_complete_cb(WriteCompleteCallback &&cb)noexcept
    {
        assert(_status == Disconnected);
        _write_complete_cb = std::move(cb);
    }

    void TcpClient::handle_connect_failed(int fd, const InetAddress &addr)
    {
        LOG_ERROR<<"connect failed";
        abort();
    }

}