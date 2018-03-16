//
// Created by lg on 17-4-21.
//
#pragma once


#include <atomic>
#include "Epoll.h"
#include<memory>

namespace net {
    class Event;

    class EventLoop final {
    public:
        EventLoop()noexcept;
        ~EventLoop()noexcept;

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator==(const EventLoop &)= delete;

        void add(Event *e);
        void update(Event* e);
        void remove(Event* e);

        void run();

        void stop();

        void handleRead();
        void wakeup();
    private:
        int _event_fd;
        std::atomic<bool> _is_looping;
        Epoll _loop;
    };


}
