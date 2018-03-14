//
// Created by lg on 17-4-21.
//
#pragma once
#include <thread>
#include"EventLoop.h"

namespace net {

    class EventLoopThread {
    public:
        EventLoopThread()noexcept;
        ~EventLoopThread()noexcept;
        EventLoopThread(const EventLoopThread&)=delete;

        void run();

        void stop();

        void join();

        EventLoop &getLoop();

    private:
        std::thread _th;
        EventLoop _loop;
    };
}
