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
#include<chrono>
#include"TimerQueue.h"

namespace net
{

    class EventLoop final
    {
    public:
        explicit EventLoop()noexcept;

        ~EventLoop()noexcept;

        EventLoop(const EventLoop &) = delete;

        EventLoop &operator==(const EventLoop &)= delete;

        void add(Event *e);

        void update(Event *e);

        void remove(Event *e);

        void run_in_loop(const std::function<void()> &cb);

        void queue_in_loop(const std::function<void()> &cb);

        void run_in_loop(std::function<void()> &&cb);

        void queue_in_loop(std::function<void()> &&cb);

        uint64_t run_after(std::chrono::milliseconds ms, const std::function<void()> &cb);

        uint64_t run_at(TimerQueue::time_point time, const std::function<void()> &cb);

        uint64_t run_every(std::chrono::milliseconds ms, const std::function<void()> &cb);

        uint64_t run_after(std::chrono::milliseconds ms, std::function<void()> &&cb);

        uint64_t run_at(TimerQueue::time_point time, std::function<void()> &&cb);

        uint64_t run_every(std::chrono::milliseconds ms, std::function<void()> &&cb);

        void cancel(uint64_t id);

        void run();

        void stop();

        bool in_loop_thread() const noexcept;

        void set_thread_id(std::thread::id id)noexcept;

    private:

        void do_pending_fn();

        void wakeup();

        void handle_wakeup_read();

    private:
        Epoll _poll;
        std::atomic<bool> _is_looping;
        std::atomic<bool> _is_pending_fns;
        int _wake_fd;
        std::thread::id _th_id;
        TimerQueue _timers;
        Event _wake_event;

        std::mutex _mu;
        std::vector<std::function<void()>> _pending_fns;

        std::vector<epoll_event> _events;
    };
}
