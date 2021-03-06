#include <unistd.h>
#include "TcpServer.h"
#include"Acceptor.h"
#include "Log.h"
#include"TcpConnection.h"

namespace net
{
    using std::placeholders::_1;
    using std::placeholders::_2;

    TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr, const std::string &name, size_t threadSize)
            :TcpServer(loop,addr,SOMAXCONN,name,threadSize)
    {
        
    }

    TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr,int backlog, const std::string &name, size_t threadSize)
            : _loop(loop)
              , _pool(loop,threadSize)
              ,_name(name)
              , _th_size(threadSize)
              , _status(Stopped)
    {
        LOG_TRACE << "server:"<<_name;
        add_acceptor(addr,backlog);
    }

    void TcpServer::add_acceptor(const InetAddress &addr,int backlog)
    {
        assert(_status == Stopped);
        LOG_TRACE;

        _accepters.emplace_back(_loop,addr,backlog);
        _accepters.back().set_new_connection_cb([this](int fd, const InetAddress &addr){handle_new_connection(fd,addr);});

        LOG_TRACE<<_accepters.size();
    }

    TcpServer::~TcpServer() noexcept
    {
        assert(_status == Stopped);
    }

    void TcpServer::run()
    {
        LOG_TRACE;
        assert(_status == Stopped);

        Status t = Stopped;
        if (_status.compare_exchange_strong(t, Running)) {
            _pool.run();
            for (auto &t:_accepters)
                t.listen();
        }
    }

    void TcpServer::stop()
    {
        LOG_TRACE;
        assert(_status == Running);

        Status t = Running;

        if (_status.compare_exchange_strong(t, Stopping)) {
            _loop->run_in_loop([this]{stop_in_loop();});
        }
    }

    void TcpServer::stop_in_loop()
    {
        assert(_loop->in_loop_thread());
        assert(_status == Stopping);

        for (auto &t:_accepters)
            t.stop();

        for (auto &conn:_connections) {
            conn.second->force_close(false);
        }
        _connections.clear();
        _pool.stop();
        _pool.join();

        _status = Stopped;
        assert(_connections.empty());
    }

    void TcpServer::handle_new_connection(int fd, const InetAddress &addr)
    {
        assert(_loop->in_loop_thread());

        if (_status != Running) {
            LOG_INFO << "server is stopping";
            return;
        }
        Socket::set_keep_alive(fd, true);

        EventLoop *loop = next_loop();

        //TCPConnPtr conn(new TcpConnection(_next_conn_id++, loop, fd, _addr, addr));
        TCPConnPtr conn {std::make_shared<TcpConnection>(_next_conn_id++, loop, fd, _addr, addr)};

        conn->set_message_cb(_message_cb);
        conn->set_connection_cb(_connecting_cb);
        conn->set_write_complete_cb(_write_complete_cb);

        conn->set_close_cb([this](const TCPConnPtr &conn){remove_connection(conn);});

        loop->run_in_loop([conn]{conn->attach_to_loop();});
        _connections[conn->id()] = conn;
    }

    void TcpServer::remove_connection(const TCPConnPtr &conn)
    {
        _loop->run_in_loop([this,conn]{remove_connection_in_loop(conn);});
    }

    void TcpServer::remove_connection_in_loop(const TCPConnPtr &conn)
    {
        assert(_loop->in_loop_thread());

        _connections.erase(conn->id());

        LOG_TRACE << "remove id=" << conn->id();
    }

    EventLoop *TcpServer::next_loop()
    {
        return _pool.next_loop();
    }

    void TcpServer::set_connection_cb(const ConnectingCallback &cb)
    {
        assert(_status==Stopped);
        _connecting_cb = cb;
    }

    void TcpServer::set_message_cb(const MessageCallback &cb)
    {
        assert(_status==Stopped);
        _message_cb = cb;
    }

    void TcpServer::set_write_complete_cb(const WriteCompleteCallback &cb)
    {
        assert(_status==Stopped);
        _write_complete_cb = cb;
    }

    void TcpServer::set_connection_cb(ConnectingCallback &&cb) noexcept
    {
        assert(_status==Stopped);
        _connecting_cb = std::move(cb);
    }

    void TcpServer::set_message_cb(MessageCallback &&cb) noexcept
    {
        assert(_status==Stopped);
        _message_cb = std::move(cb);
    }

    void TcpServer::set_write_complete_cb(WriteCompleteCallback &&cb) noexcept
    {
        assert(_status==Stopped);
        _write_complete_cb = std::move(cb);
    }

    const std::string &TcpServer::name() const {
        return _name;
    }

    EventLoop *TcpServer::loop() noexcept {
        return _loop;
    }

    void TcpServer::set_thread_size(size_t s) {
        assert(_th_size==0);
        assert(_status == Stopped);

        _th_size=s;
        _pool.resize(s);
    }

    size_t TcpServer::size() const noexcept {
        return _pool.size();
    }
}
