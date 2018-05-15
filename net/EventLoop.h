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
        EventLoop(EventLoop&&) = delete;

        /// 线程安全
        /// 如果当前线程是本对象所在线程，则立即执行cb
        /// 如果不是，则将cb加入事件队列，等待在loop循环中被执行
        /// 加入队列会唤醒loop循环
        void run_in_loop(const EventCallback &cb);

        /// 线程安全
        /// 将cb加入事件队列，等待在loop循环中被执行
        /// 加入队列会唤醒loop循环
        void queue_in_loop(const EventCallback &cb);

        void run_in_loop(EventCallback &&cb);

        void queue_in_loop(EventCallback &&cb);

        /// 设置一个定时器
        uint64_t run_after(std::chrono::milliseconds ms, const EventCallback &cb);

        uint64_t run_at(impl::TimerQueue::time_point time, const EventCallback &cb);

        uint64_t run_every(std::chrono::milliseconds ms, const EventCallback &cb);

        uint64_t run_after(std::chrono::milliseconds ms, EventCallback &&cb);

        uint64_t run_at(impl::TimerQueue::time_point time, EventCallback &&cb);

        uint64_t run_every(std::chrono::milliseconds ms, EventCallback &&cb);

        /// 取消定时器
        void cancel(uint64_t id);

        void run();

        void stop();

        /// 判断当前线程是不是loop所在线程
        bool in_loop_thread() const noexcept;

    public: ///以下函数不应被用户使用

        /// 添加事件
        void add(impl::Event *e);

        /// 更新已添加的事件
        void update(impl::Event *e);

        /// 删除已添加事件
        void remove(impl::Event *e);

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
