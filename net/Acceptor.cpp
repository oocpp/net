//
// Created by lg on 17-4-19.
//

#include <cstring>
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Log.h"

namespace net {
    Accepter::Accepter(EventLoop *loop, const InetAddress &addr)
            :_loop(loop)
            ,_fd(Socket::create_nonblocking_socket())
            ,_addr(addr)
            ,_event(loop,_fd,true,false){
        LOG_INFO<<"true="<< true;
    }

    Accepter::~Accepter() {
        Socket::close(_fd);
    }

    void Accepter::listen(int backlog) {
        Socket::bind(_fd, _addr);
        Socket::listen(_fd, backlog);

        _event.set_read_cb(std::bind(&Accepter::handle_accept, this));

        _loop->run_in_loop(std::bind(&Event::attach_to_loop,&_event));
    }

    void Accepter::stop() {
        _event.detach_from_loop();
    }

    void Accepter::handle_accept() {
        InetAddress addr;
        int connfd = Socket::accept(_fd,addr);
        _new_connection_cb(connfd,addr);
    }


}