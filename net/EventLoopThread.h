//
// Created by lg on 17-4-21.
//
#pragma once
#include <thread>
#include"EventLoop.h"

namespace net {

    class EventLoopThread {
    public:
        EventLoopThread();
        ~EventLoopThread();
        EventLoopThread(const EventLoopThread&)=delete;

        void run();

        void stop();

        void join();

    private:
        std::thread _th;
        EventLoop _loop;
    };
}
