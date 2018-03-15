//
// Created by lg on 18-3-13.
//

#include "Server.h"
#include"Acceptor.h"

namespace net {
    Server::Server(EventLoop *loop, const InetAddress &addr, const std::string &name, size_t threadSize) : _pool(
            threadSize), _loop_ptr(loop) {

    }

    void Server::run() {
        _accepter->accept();
        _pool.run();
    }

    void Server::init() {
        _accepter->listen();
    }

    void Server::stop() {
        _accepter->stop();
        _pool.stop();
    }
}