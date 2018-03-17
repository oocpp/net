//
// Created by lg on 17-4-21.
//
#pragma once


#include <atomic>
#include "Epoll.h"
#include<memory>
#include<vector>
#include"Event.h"
#include<mutex>

namespace net {

    class EventLoop final {
    public:
        EventLoop()noexcept;
        ~EventLoop()noexcept;

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator==(const EventLoop &)= delete;

        void add(Event *e);
        void update(Event* e);
        void remove(Event* e);
        void runInLoop(const std::function<void()>&cb);

        bool isInLoopThread();

        void run();

        void stop();

        void wakeup();
        void handleWakeRead();
    private:

        Epoll _loop;
        int _wake_fd;
        Event _wake_event;

        std::atomic<bool> _is_looping;

        std::mutex _mu;
        std::vector<std::function<void()>> _pendingFunctors;
    };


}
