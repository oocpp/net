//
// Created by lg on 17-4-21.
//

#include <sys/socket.h>
#include <unistd.h>
#include "EventLoop.h"

#include "Log.h"
#include "EventLoopThread.h"
#include "Acceptor.h"
#include "Socket.h"

namespace net {
    EventLoop::~EventLoop()noexcept {
        _loop.eventDel(_fd_pair[1]);
        Socket::close(_fd_pair[0]);
        Socket::close(_fd_pair[1]);
    }

    void EventLoop::add(std::unique_ptr<Accepter> &acc) {

    }

    void EventLoop::add(std::shared_ptr<TcpConnection> &conn) {

    }

    void EventLoop::erase(std::shared_ptr<TcpConnection> &acc) {

    }

    void EventLoop::erase(std::unique_ptr<Accepter> &) {

    }

    void EventLoop::run() {
        while(_is_looping){
            LOG_TRACE<<"looping"<<std::endl;
            _loop.wait(-1);
            LOG_TRACE<<"loop stop"<<std::endl;
        }
    }

    void EventLoop::stop() {
        _is_looping=false;
        ::write(_fd_pair[0],"A",1);
    }

    void EventLoop::init() {
        _loop.init();

        if(socketpair(AF_LOCAL, SOCK_STREAM, 0, _fd_pair)== -1)
        {
            LOG_ERROR<<"socket pair 失败";
        }

        epoll_event e;
        e.events=Epoll::READ;
        _loop.eventAdd(_fd_pair[1],e);

        _is_looping=true;
    }

    EventLoop::EventLoop()noexcept:_is_looping(false),_fd_pair{-1,-1} {

    }


}
