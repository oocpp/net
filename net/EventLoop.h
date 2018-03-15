//
// Created by lg on 17-4-21.
//
#pragma once


#include <atomic>
#include "Epoll.h"
#include<memory>

namespace net {
    class Accepter;
    class TcpConnection;

    class EventLoop final {
    public:
        EventLoop()noexcept;
        ~EventLoop()noexcept;

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator==(const EventLoop &)= delete;

        void add(std::shared_ptr<TcpConnection> &acc);

        void add(std::unique_ptr<Accepter> &);

        void erase(std::shared_ptr<TcpConnection> &acc);

        void erase(std::unique_ptr<Accepter> &);

        void run();

        void stop();

        void handleRead();
        void wakeup();
    private:
        int _event_fd;
        std::atomic<bool> _is_looping;
        Epoll _loop;
    };


}
