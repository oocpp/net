//
// Created by lg on 18-3-13.
//

#include "TcpServer.h"
#include"Acceptor.h"
#include "Log.h"
#include"TcpConnection.h"

namespace net
{
    using std::placeholders::_1;
    using std::placeholders::_2;

    TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr, const std::string &name, size_t threadSize)
            : _loop(loop)
              , _pool(threadSize)
              , _accepter(loop, addr)
              , _th_size(threadSize)
              , _status(Init)
    {
        LOG_TRACE << "server";
        _accepter.set_new_connection_cb(std::bind(&TcpServer::handle_new_connection, this, _1, _2));
    }

    TcpServer::~TcpServer() noexcept
    {
        assert(_status == Stopped);
    }

    void TcpServer::run()
    {
        assert(_status == Init);

        Status t = Init;
        if (_status.compare_exchange_strong(t, Running)) {
            _pool.run();
            _accepter.listen();
        }
    }

    void TcpServer::stop()
    {
        assert(_status == Running);

        Status t = Running;

        if (_status.compare_exchange_strong(t, Stopping)) {
            _loop->run_in_loop(std::bind(&TcpServer::stop_in_loop, this));
            _pool.join();
        }
        _status = Stopped;
    }

    void TcpServer::stop_in_loop()
    {
        assert(_loop->in_loop_thread());
        assert(_status == Stopping);

        _accepter.stop();

        for (auto &conn:_connections) {
            conn.second->close();
        }
        _pool.stop();
    }

    void TcpServer::handle_new_connection(int fd, const InetAddress &addr)
    {
        assert(_loop->in_loop_thread());

        if (_status != Running) {
            LOG_INFO << "server is stopping";
            return;
        }

        LOG_TRACE << "get_fd=" << fd;

        EventLoop *loop = get_next_loop();


        TCPConnPtr conn(new TcpConnection(_next_conn_id++, loop, fd, _addr, addr));

        conn->set_message_cb(_message_cb);
        conn->set_connection_cb(_connecting_cb);
        conn->set_write_complete_cb(_write_complete_cb);
        conn->set_close_cb(std::bind(&TcpServer::remove_connection, this, _1));

        loop->run_in_loop(std::bind(&TcpConnection::attach_to_loop, conn));
        _connections[conn->get_id()] = conn;
    }

    void TcpServer::remove_connection(const TCPConnPtr &conn)
    {

        _loop->run_in_loop(std::bind(&TcpServer::remove_connection_in_loop, this, conn));
    }

    void TcpServer::remove_connection_in_loop(const TCPConnPtr &conn)
    {
        assert(_loop->in_loop_thread());

        _connections.erase(conn->get_id());
        //EventLoop* loop = conn->get_loop();
        //loop->queue_in_loop(std::bind(&TcpConnection::connectDestroyed, conn));

        LOG_TRACE << "remove id=" << conn->get_id();
    }

    EventLoop *TcpServer::get_next_loop()
    {
        if (_th_size > 0)
            return _pool.get_next_loop();
        return _loop;

    }

    void TcpServer::set_connection_cb(const ConnectingCallback &cb)
    {
        _connecting_cb = cb;
    }

    void TcpServer::set_message_cb(const MessageCallback &cb)
    {
        _message_cb = cb;
    }

    void TcpServer::set_write_complete_cb(const WriteCompleteCallback &cb)
    {
        _write_complete_cb = cb;
    }

    void TcpServer::set_connection_cb(ConnectingCallback &&cb) noexcept
    {
        _connecting_cb = std::move(cb);
    }

    void TcpServer::set_message_cb(MessageCallback &&cb) noexcept
    {
        _message_cb = std::move(cb);
    }

    void TcpServer::set_write_complete_cb(WriteCompleteCallback &&cb) noexcept
    {
        _write_complete_cb = std::move(cb);
    }

}