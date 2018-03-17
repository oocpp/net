//
// Created by lg on 18-3-13.
//

#include "Server.h"
#include"Acceptor.h"
#include "Log.h"
#include"TcpConnection.h"

namespace net {
    using std::placeholders::_1;
    using std::placeholders::_2;

    Server::Server(EventLoop *loop, const InetAddress &addr, const std::string &name, size_t threadSize)
            : _pool(threadSize)
            , _loop_ptr(loop)
            ,_accepter(loop,addr){

    }

    void Server::run() {
        _pool.run();

        _accepter.set_new_connection_cb(std::bind(&Server::handle_new_connection, this, _1, _2));
        _accepter.listen();
    }

    void Server::stop() {
        _accepter.stop();
        _pool.stop();
    }

    void Server::handle_new_connection(int fd, const InetAddress &addr) {
        LOG_TRACE << "get_fd=" << fd;


        EventLoop* io_loop = _pool.next_loop();


        TCPConnPtr conn(new TcpConnection(_next_conn_id++,io_loop, fd,addr));
        conn->set_message_cb(_message_cb);
        //conn->setConnectionCallback(conn_fn_);
        //conn->setCloseCallback(std::bind(&TCPServer::RemoveConnection, this, std::placeholders::_1));
        //io_loop->run_in_loop(std::bind(&TCPConn::OnAttachedToLoop, conn));
        conn->attach_to_loop();
        _connections[conn->get_id()] = conn;
    }
}