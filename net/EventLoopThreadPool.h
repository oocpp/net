//
// Created by lg on 18-3-13.
//

#pragma once

#include "EventLoopThread.h"
#include"Connector.h"
#include"Acceptor.h"
#include<vector>

namespace net{

    class EventLoopThreadPool {
    public:
        EventLoopThreadPool(size_t threadNum=1):_threads(threadNum),_loop_index(0){

        }

        void start();
        void stop();
        void join();

        void add(Accepter&acc);
        void add(Connector&conn);
    private:
        std::vector<EventLoopThread>_threads;
        size_t _loop_index;
    };

}



