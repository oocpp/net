//
// Created by lg on 17-4-19.
//

#include <cstring>
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Log.h"

namespace net {
    Accepter::~Accepter() {
        Socket::close(_fd);
    }

    void Accepter::listen(int backlog) {
        _fd = Socket::createNonblockingSocket();

        if (_fd < 0)
            return;

        Socket::bind(_fd, _addr);
        Socket::listen(_fd, backlog);
    }

    void Accepter::accept() {

    }

    void Accepter::stop() {

    }

}