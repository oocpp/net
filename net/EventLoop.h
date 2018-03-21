//
// Created by lg on 17-4-21.
//
#pragma once


#include <atomic>
#include "Epoll.h"
#include<memory>
#include<vector>
#include"Event.h"
#include<mutex>
#include <thread>

namespace net {

    class EventLoop final {
    public:
        EventLoop()noexcept;
        ~EventLoop()noexcept;

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator==(const EventLoop &)= delete;

        void add(Event *e);
        void update(Event* e);
        void remove(Event* e);

        void run_in_loop(const std::function<void()> &cb);
        void queue_in_loop(const std::function<void()> &cb);

        void run();

        void stop();
        bool in_loop_thread()const;
    private:

        void do_pending_fn();

        void wakeup();
        void handle_wakeup_read();
    private:
        Epoll _poll;
        int _wake_fd;
        Event _wake_event;
        std::thread::id _th_id;

        std::atomic<bool> _is_looping;

        std::mutex _mu;
        std::vector<std::function<void()>> _pending_fns;
        std::atomic<bool> _is_pending_fns;

        std::vector<epoll_event>_events;
    };


}
