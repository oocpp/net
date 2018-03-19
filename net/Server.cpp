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
        LOG_TRACE<<"server";
        _accepter.set_new_connection_cb(std::bind(&Server::handle_new_connection, this, _1, _2));
    }

    void Server::run() {
        _pool.run();
        _accepter.listen();
    }

    void Server::stop() {
        _accepter.stop();
        _pool.stop();
    }

    void Server::handle_new_connection(int fd, const InetAddress &addr) {
        LOG_TRACE << "get_fd=" << fd;


        EventLoop* loop = _pool.get_next_loop();


        TCPConnPtr conn(new TcpConnection(_next_conn_id++,loop, fd,_addr,addr));
        conn->set_message_cb(_message_cb);
        //conn->set_connection_cb(_connecting_cb);
        //conn->set_closeCallback(std::bind(&TCPServer::RemoveConnection, this, std::placeholders::_1));
        loop->run_in_loop(std::bind(&TcpConnection::attach_to_loop, conn));
        _connections[conn->get_id()] = conn;
    }
}