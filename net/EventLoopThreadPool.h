//
// Created by lg on 18-3-13.
//

#pragma once
#include "EventLoopThread.h"
#include<vector>
#include <cstddef>

namespace net{

    class EventLoop;

    class EventLoopThreadPool {
    public:
        EventLoopThreadPool(size_t threadNum=0);

        void run();
        void stop();
        void join();

        EventLoop&getNextLoop();
    private:
        size_t nextLoopIndex();

        std::vector<EventLoopThread>_threads;
        size_t _loop_index;
    };

}



