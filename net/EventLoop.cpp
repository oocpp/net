//
// Created by lg on 17-4-21.
//

#include <sys/socket.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include "EventLoop.h"

#include "Log.h"
#include "EventLoopThread.h"
#include "Acceptor.h"
#include "Socket.h"

namespace net {
    EventLoop::~EventLoop()noexcept {
        _loop.remove(_event_fd);
        Socket::close(_event_fd);
    }

    EventLoop::EventLoop()noexcept:_is_looping(true),_event_fd(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) {

        if (_event_fd < 0) {
            LOG_ERROR << "Failed in eventfd";
        }

        epoll_event e;
        e.events=Epoll::READ;
        _loop.add(_event_fd,e);
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
        wakeup();
    }

    void EventLoop::wakeup()
    {
        uint64_t one = 1;
        ssize_t n = ::write(_event_fd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
        }
    }

    void EventLoop::handleRead()
    {
        uint64_t one = 1;
        ssize_t n = ::read(_event_fd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
    }

}
