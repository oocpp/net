#pragma once

#include "EventLoopThread.h"
#include<vector>
#include <cstddef>
#include <cassert>

namespace net
{
    class EventLoop;

    class EventLoopThreadPool
    {
    public:
        /// threadSize 为0，next_loop永远返回loop
        explicit EventLoopThreadPool(EventLoop*loop,size_t threadNum = 0);

        EventLoopThreadPool(const EventLoopThreadPool &) = delete;
        EventLoopThreadPool &operator==(const EventLoopThreadPool &)= delete;

        void run();

        void stop();

        void join();

        void resize(size_t s);

        size_t size() const noexcept;

        /// 获取一个loop
        /// threadNum为0，则返回_loop
        EventLoop *next_loop();

    private:
        size_t next_loop_index();

        EventLoop*_loop;
        std::vector<EventLoopThread> _threads;
        size_t _loop_index;
    };

}



