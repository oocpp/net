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
        _wake_event.detach_from_loop();
        Socket::close(_wake_fd);
    }

    EventLoop::EventLoop()noexcept
            :_is_looping(true)
             ,_is_pending_fns(false)
            ,_wake_fd(createWakeEventfd())
            ,_wake_event(this,_wake_fd,true)
            ,_th_id(std::this_thread::get_id()){

        _wake_event.set_read_cb(std::bind(&EventLoop::handle_wakeup_read, this));
        _wake_event.attach_to_loop();
    }

    void EventLoop::run() {
        while(_is_looping){
            LOG_TRACE<<"looping"<<std::endl;

            _poll.wait(-1,_events);

            for(auto &e:_events)
                reinterpret_cast<Event*>(e.data.ptr)->handle_event(e.events);

            do_pending_fn();
            LOG_TRACE<<"get_loop stop"<<std::endl;
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

    void EventLoop::handle_wakeup_read()
    {
        uint64_t one = 1;
        ssize_t n = ::read(_wake_fd, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
    }

    void EventLoop::add(Event *e) {
        epoll_event event;
        event.events=e->get_events();
        event.data.ptr=e;
       _poll.add(e->get_fd(),event);
    }

    void EventLoop::update(Event *e) {
        epoll_event event;
        event.events=e->get_events();
        event.data.ptr=e;
        _poll.update(e->get_fd(),event);
    }

    void EventLoop::remove(Event *e) {
        _poll.remove(e->get_fd());
    }

    void EventLoop::run_in_loop(const std::function<void()> &cb) {
        if(in_loop_thread()){
            cb();
        }
        else {
            queue_in_loop(cb);
        }
    }

    bool EventLoop::in_loop_thread()const {
        return std::this_thread::get_id()==_th_id;
    }

    void EventLoop::queue_in_loop(const std::function<void()> &cb) {
        {
            std::lock_guard<std::mutex> l(_mu);
            _pending_fns.push_back(cb);
        }

        if(!in_loop_thread()||_is_pending_fns) {
            wakeup();
        }
    }

    void EventLoop::do_pending_fn() {
        std::vector<std::function<void()>>fns;
        _is_pending_fns=true;
        {
            std::lock_guard<std::mutex> l(_mu);
            fns.swap(_pending_fns);
        }
        for(auto&f:fns)
            f();
        _is_pending_fns=false;
    }
}
