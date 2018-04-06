//
// Created by lg on 18-3-13.
//

#pragma once

#include "EventLoopThread.h"
#include<vector>
#include <cstddef>

namespace net
{
    class EventLoop;

    class EventLoopThreadPool
    {
    public:
        EventLoopThreadPool(size_t threadNum = 0);

        EventLoopThreadPool(const EventLoopThreadPool &) = delete;
        EventLoopThreadPool &operator==(const EventLoopThreadPool &)= delete;

        void run();

        void stop();

        void join();

        EventLoop *get_next_loop();

    private:
        size_t next_loop_index();

        std::vector<EventLoopThread> _threads;
        size_t _loop_index;
    };

}



