//
// Created by lg on 18-3-13.
//

#pragma once

#include"EventLoopThreadPool.h"


namespace net{
    class EventLoop;

    class Server {
    public:
        Server(EventLoop*loop,size_t threadSize=0);

        void run();
    private:
        EventLoop*_loop_ptr;
        EventLoopThreadPool _pool;
    };

}

