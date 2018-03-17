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

    Server::Server(EventLoop *loop, const InetAddress &addr, const std::string &name, size_t threadSize) : _pool(
            threadSize), _loop_ptr(loop) {

    }

    void Server::run() {
        _accepter->accept();
        _accepter->setNewConnectedCallBack(std::bind(&Server::handleNewConnection,this,_1,_2));
        _pool.run();
    }

    void Server::init() {
        _accepter->listen();
    }

    void Server::stop() {
        _accepter->stop();
        _pool.stop();
    }

    void Server::handleNewConnection(int fd, const InetAddress &addr) {
        LOG_TRACE << "fd=" << fd;


        EventLoop* io_loop = _pool.getNextLoop();


        TCPConnPtr conn(new TcpConnection(_next_conn_id++,io_loop, fd,addr));
        //conn->setMessageCallback(msg_fn_);
        //conn->setConnectionCallback(conn_fn_);
        //conn->setCloseCallback(std::bind(&TCPServer::RemoveConnection, this, std::placeholders::_1));
        //io_loop->runInLoop(std::bind(&TCPConn::OnAttachedToLoop, conn));
        //conn->add
        _connections[conn->id()] = conn;
    }
}