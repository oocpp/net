#pragma once
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include "Epoll.h"
#include "CallBack.h"
#include "Event.h"
#include "TimerQueue.h"

namespace net
{
    class EventLoop final
    {
    public:
        explicit EventLoop()noexcept;

        ~EventLoop()noexcept;

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator==(const EventLoop &)= delete;

        void add(impl::Event *e);

        void update(impl::Event *e);

        void remove(impl::Event *e);

        void run_in_loop(const EventCallback &cb);

        void queue_in_loop(const EventCallback &cb);

        void run_in_loop(EventCallback &&cb);

        void queue_in_loop(EventCallback &&cb);

        uint64_t run_after(std::chrono::milliseconds ms, const EventCallback &cb);

        uint64_t run_at(impl::TimerQueue::time_point time, const EventCallback &cb);

        uint64_t run_every(std::chrono::milliseconds ms, const EventCallback &cb);

        uint64_t run_after(std::chrono::milliseconds ms, EventCallback &&cb);

        uint64_t run_at(impl::TimerQueue::time_point time, EventCallback &&cb);

        uint64_t run_every(std::chrono::milliseconds ms, EventCallback &&cb);

        void cancel(uint64_t id);

        void run();

        void stop();

        bool in_loop_thread() const noexcept;

        void reset_thread_id()noexcept;

    private:

        void do_pending_fn();

        void wakeup();

        void handle_wakeup_read();

    private:
        impl::Epoll _poll;
        std::atomic<bool> _is_looping;
        std::atomic<bool> _is_pending_fns;
        int _wake_fd;
        std::thread::id _th_id;
        impl::TimerQueue _timers;
        impl::Event _wake_event;

        std::mutex _mu;
        std::vector<EventCallback> _pending_fns;

        std::vector<epoll_event> _events;
    };
}
