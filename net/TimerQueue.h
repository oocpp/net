#pragma once


#include <set>
#include<map>
#include <vector>
#include<chrono>
#include<memory>
#include "Event.h"
#include<memory>
#include"Timer.h"

namespace net {

    class EventLoop;


    class TimerQueue {
    public:
        using time_point = Timer::time_point;
        using duration = std::chrono::system_clock::duration;

        explicit TimerQueue(EventLoop *loop);

        ~TimerQueue();


        uint64_t addTimer(const TimerCallback &cb,
                         time_point when,
                         std::chrono::milliseconds interval);

        void cancel(uint64_t timerId);

        static time_point now(){
            return std::chrono::system_clock::now();
        }

        using Entry= std::pair<time_point, Timer*>;
    private:

        typedef std::set<Entry> TimerList;

        typedef std::map<uint64_t ,Timer*>ActiveTimerSet ;

        void addTimerInLoop(Timer* timer);

        void cancelInLoop(uint64_t timerId);

        // called when timerfd alarms
        void handleRead();

        // move out all expired timers
        std::vector<Entry> getExpired(time_point now);

        void reset(const std::vector<Entry> &expired, time_point now);

        bool insert(Timer* timer);

        EventLoop *loop_;
        const int timerfd_;
        Event timerfdChannel_;
        // Timer list sorted by expiration
        TimerList timers_;

        // for cancel()
        ActiveTimerSet activeTimers_;
        bool callingExpiredTimers_; /* atomic */
        std::set<uint64_t > cancelingTimers_;
    };

}
