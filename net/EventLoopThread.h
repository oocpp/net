#pragma once

#include <thread>
#include"EventLoop.h"

namespace net
{

    class EventLoopThread
    {
    public:
        EventLoopThread()noexcept;

        ~EventLoopThread()noexcept;

        EventLoopThread(const EventLoopThread &) = delete;
        EventLoopThread &operator==(const EventLoopThread &)= delete;

        void run();

        void stop();

        void join();

        void stop_and_join();

        EventLoop *get_loop();

    private:

        void thread_fn();

        std::thread _th;
        EventLoop _loop;
    };
}
