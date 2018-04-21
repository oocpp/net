#pragma once

#include <set>
#include <map>
#include <vector>
#include <chrono>
#include <memory>

#include "Event.h"
#include "Timer.h"

namespace net
{
    class EventLoop;

    namespace impl
    {
        class TimerQueue
        {
        public:
            using time_point = Timer::time_point;
            using duration = std::chrono::system_clock::duration;

            explicit TimerQueue(EventLoop *loop);

            ~TimerQueue()noexcept;

            TimerQueue(const TimerQueue &) = delete;

            TimerQueue &operator==(const TimerQueue &)= delete;

            uint64_t add_timer(const EventCallback &cb, time_point when, std::chrono::milliseconds interval);

            uint64_t add_timer(EventCallback &&cb, time_point when, std::chrono::milliseconds interval);

            void cancel(uint64_t timerId);

            static time_point now();

            using TimerNode= std::pair<time_point, Timer *>;
        private:
            typedef std::set<TimerNode> TimerList;

            typedef std::map<uint64_t, Timer *> ActiveTimerMap;

            timespec from_now(time_point when);

            void read_timer(time_point now);

            void reset_timer(time_point expiration);

            void add_timer_in_loop(Timer *timer);

            void cancel_in_loop(uint64_t timerId);

            void handle_expire();

            std::vector<TimerNode> get_expired(time_point now);

            void reset(const std::vector<TimerNode> &expired, time_point now);

            bool insert(Timer *timer);

        private:
            EventLoop *_loop;
            const int _timer_fd;
            Event _timer_event;
            TimerList _timers;

            ActiveTimerMap _active_timers;
            bool _calling_expired_timers;
            std::set<uint64_t> _canceling_timers;
        };
    }
}
