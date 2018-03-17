//
// Created by lg on 18-3-16.
//

#pragma once
#include<functional>
#include <sys/epoll.h>

namespace net {
    class EventLoop;

    class Event {
    public:
        Event(EventLoop*loop,int fd,bool watch_read=false, bool watch_write=false)noexcept;

        static constexpr int NoneEvent=0;
        static constexpr int ReadEvent=EPOLLIN|EPOLLPRI;
        static constexpr int WriteEvent=EPOLLOUT;

        typedef std::function<void()> EventCallback;
        typedef std::function<void()> ReadEventCallback;

        void set_read_cb(const ReadEventCallback &cb) {
            _read_fn = cb;
        }

        void set_write_cb(const EventCallback &cb) {
            _write_fn = cb;
        }

        void attach_to_loop();
        void detach_from_loop();

        int get_fd()const{return _fd;}
        int get_events()const{return _events;}
        void set_active_events(int event){_active_event=event;}

    private:
        int _fd;
        EventLoop* _loop;
        int _events;
        int _active_event;
        ReadEventCallback _read_fn;
        EventCallback _write_fn;
    };
}

