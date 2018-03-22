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
            ,_fd(Socket::create_nonblocking_socket(addr.get_family()))
            ,_addr(addr)
            ,_event(loop,_fd,true,false){
        LOG_INFO<<"true="<< true;
        Socket::bind(_fd, _addr);
    }

    Accepter::~Accepter() {
        assert(!_event.is_add_to_loop());

        Socket::close(_fd);
    }

    void Accepter::listen(int backlog) {
        Socket::listen(_fd, backlog);

        _event.set_read_cb(std::bind(&Accepter::handle_accept, this));

        _loop->run_in_loop(std::bind(&Event::attach_to_loop,&_event));
    }

    void Accepter::stop() {
        assert(_loop->in_loop_thread());

        _event.detach_from_loop();
    }

    void Accepter::handle_accept() {
        assert(_loop->in_loop_thread());

        InetAddress addr;
        int connfd = Socket::accept(_fd,addr);

        if(connfd<0){
            return ;
        }

        _new_connection_cb(connfd,addr);
    }


}