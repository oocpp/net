#include<chrono>
#include <cassert>
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "Log.h"
#include"TimerQueue.h"
#include "Socket.h"
#include"EventLoop.h"


namespace net {
    namespace {
        int createTimerfd() {
            int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                           TFD_NONBLOCK | TFD_CLOEXEC);
            if (timerfd < 0) {
                LOG_ERROR << "Failed in timerfd_create";
            }
            return timerfd;
        }

        struct timespec howMuchTimeFromNow(TimerQueue::time_point when) {

            TimerQueue::duration du = when - std::chrono::system_clock::now();

            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(std::chrono::duration_cast<std::chrono::seconds>(du).count());

            ts.tv_nsec = static_cast<long>(std::chrono::duration_cast<std::chrono::nanoseconds>(du % 1s).count());
            return ts;
        }

        void readTimerfd(int timerfd, TimerQueue::time_point now) {
            uint64_t howmany;
            ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
            LOG_TRACE << howmany;
            if (n != sizeof howmany) {
                LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
            }
        }

        void resetTimerfd(int timerfd, TimerQueue::time_point expiration) {
            // wake up loop by timerfd_settime()
            struct itimerspec newValue;
            struct itimerspec oldValue;
            bzero(&newValue, sizeof newValue);
            bzero(&oldValue, sizeof oldValue);
            newValue.it_value = howMuchTimeFromNow(expiration);
            int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
            if (ret) {
                LOG_ERROR << "timerfd_settime()";
            }
        }
    }


    TimerQueue::TimerQueue(EventLoop *loop)
            : loop_(loop),
              timerfd_(createTimerfd()),
              timerfdChannel_(loop, timerfd_),
              timers_(),
              callingExpiredTimers_(false) {

        timerfdChannel_.set_read_cb(std::bind(&TimerQueue::handleRead, this));
        timerfdChannel_.enable_read();
    }

    TimerQueue::~TimerQueue() noexcept{
        timerfdChannel_.disable_all();
        Socket::close(timerfd_);

        for (TimerList::iterator it = timers_.begin(); it != timers_.end(); ++it) {
            delete it->second;
        }
    }

    uint64_t TimerQueue::addTimer(const TimerCallback &cb,
                                 time_point when,
                                 std::chrono::milliseconds interval) {
        Timer* timer=new Timer(cb, when, interval);

        loop_->run_in_loop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
        return timer->sequence();
    }


    void TimerQueue::cancel(uint64_t timerId) {
        loop_->run_in_loop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
    }

    void TimerQueue::addTimerInLoop(Timer* timer) {

        bool earliestChanged = insert(timer);

        if (earliestChanged) {
            resetTimerfd(timerfd_, timer->expiration());
        }
    }

    void TimerQueue::cancelInLoop(uint64_t timerId) {

        assert(timers_.size() == activeTimers_.size());

        auto it = activeTimers_.find(timerId);
        if (it != activeTimers_.end()) {
            timers_.erase(Entry(it->second->expiration(), it->second));

            activeTimers_.erase(it);
        } else if (callingExpiredTimers_) {
            cancelingTimers_.insert(timerId);
        }

        assert(timers_.size() == activeTimers_.size());
    }

    void TimerQueue::handleRead() {

        time_point now_time(now());
        readTimerfd(timerfd_, now_time);

        std::vector<Entry> expired = getExpired(now_time);

        callingExpiredTimers_ = true;
        cancelingTimers_.clear();

        for (auto it = expired.begin(); it != expired.end(); ++it) {
            it->second->run();
        }
        callingExpiredTimers_ = false;

        reset(expired, now_time);
    }

    std::vector<TimerQueue::Entry> TimerQueue::getExpired(time_point now) {
        assert(timers_.size() == activeTimers_.size());

        std::vector<Entry> expired;

        Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));

        TimerList::iterator end = timers_.lower_bound(sentry);

        assert(end == timers_.end() || now < end->first);

        std::copy(timers_.begin(), end, back_inserter(expired));
        timers_.erase(timers_.begin(), end);

        for (auto it = expired.begin(); it != expired.end(); ++it) {

            size_t n = activeTimers_.erase(it->second->sequence());
            assert(n == 1);
        }

        assert(timers_.size() == activeTimers_.size());
        return expired;
    }

    void TimerQueue::reset(const std::vector<Entry> &expired, time_point now) {
        time_point nextExpire;

        for (auto it = expired.begin(); it != expired.end(); ++it) {

            if (it->second->repeat() && cancelingTimers_.find(it->second->sequence()) == cancelingTimers_.end()) {
                it->second->restart(now);
                insert(it->second);
            } else {
                // FIXME move to a free list
                delete it->second; // FIXME: no delete please
            }
        }

        if (!timers_.empty()) {
            nextExpire = timers_.begin()->second->expiration();
        }

        if (nextExpire > time_point{}) {
            resetTimerfd(timerfd_, nextExpire);
        }
    }

    bool TimerQueue::insert(Timer* timer) {

        assert(timers_.size() == activeTimers_.size());

        bool earliestChanged = false;

        time_point when = timer->expiration();

        auto it = timers_.begin();
        if (it == timers_.end() || when < it->first) {
            earliestChanged = true;
        }

        {
            std::pair<TimerList::iterator, bool> result = timers_.insert(Entry(when, timer));
            assert(result.second);
            (void) result;
        }
        {
            std::pair<ActiveTimerSet::iterator, bool> result
                    = activeTimers_.insert(std::pair<uint64_t ,Timer*>( timer->sequence(),timer));
            assert(result.second);
            (void) result;
        }

        assert(timers_.size() == activeTimers_.size());
        return earliestChanged;
    }

    TimerQueue::time_point TimerQueue::now() {
        return std::chrono::system_clock::now();
    }
}

