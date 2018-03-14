//
// Created by lg on 17-4-21.
//
#pragma once


#include <atomic>
#include "Epoll.h"
#include<memory>
#include<condition_variable>
#include<mutex>

namespace net {


    class Accepter;

    class TcpConnection;

    class EventLoop final {
    public:
        EventLoop();
        ~EventLoop();

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator==(const EventLoop &)= delete;


        void add(std::shared_ptr<TcpConnection> &acc);

        void add(std::unique_ptr<Accepter> &);

        void erase(std::shared_ptr<TcpConnection> &acc);

        void erase(std::unique_ptr<Accepter> &);

        void run();

        void stop();

    private:
        std::mutex _mu;
        std::condition_variable _cv;
        std::atomic<bool> _is_looping;
        Epoll _loop;
    };


}
