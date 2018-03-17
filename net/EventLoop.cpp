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
#include"Event.h"

namespace{
    int createWakeEventfd() {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0)
        {
            LOG_ERROR << "Failed in eventfd";
            //abort();
        }
        return evtfd;
    }
}

namespace net {
    EventLoop::~EventLoop()noexcept {
        _wake_event.detachFromLoop();
        Socket::close(_wake_fd);
    }

    EventLoop::EventLoop()noexcept
            :_is_looping(true)
            ,_wake_fd(createWakeEventfd())
            ,_wake_event(this,_wake_fd){

        _wake_event.SetReadCallback(std::bind(&EventLoop::handleWakeRead,this));
      _wake_event.attachToLoop();
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
        ssize_t n = ::write(_wake_fd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
        }
    }

    void EventLoop::handleWakeRead()
    {
        uint64_t one = 1;
        ssize_t n = ::read(_wake_fd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
    }

    void EventLoop::add(Event *e) {

    }

    void EventLoop::update(Event *e) {

    }

    void EventLoop::remove(Event *e) {

    }

    void EventLoop::runInLoop(const std::function<void()> &cb) {
        if(isInLoopThread()){
            cb();
        }
        else {
            std::lock_guard<std::mutex> l(_mu);
            _pendingFunctors.push_back(cb);
        }
    }

    bool EventLoop::isInLoopThread() {
        return false;
    }
}
