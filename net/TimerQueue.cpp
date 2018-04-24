#include<chrono>
#include <cassert>
#include <sys/timerfd.h>
#include <unistd.h>
#include <cstring>

#include "Log.h"
#include"TimerQueue.h"
#include "Socket.h"
#include"EventLoop.h"

namespace net
{
    namespace impl
    {
        namespace
        {
            int create_timer_fd()
            {
                int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
                if (timerfd < 0) {
                    LOG_ERROR << "Failed in timerfd_create";
                }
                return timerfd;
            }
        }

        TimerQueue::TimerQueue(EventLoop *loop)
                : _loop(loop)
                  , _timer_fd(create_timer_fd())
                  , _timer_event(loop, _timer_fd)
                  , _calling_expired_timers(false)
        {
            //_timer_event.set_read_cb(std::bind(&TimerQueue::handle_expire, this));
            _timer_event.set_read_cb([this] { handle_expire(); });
            _timer_event.enable_read();
        }

        TimerQueue::~TimerQueue() noexcept
        {
            assert(_timer_set.empty());
            assert(_timer_map.empty());
            assert(_canceling_timers.empty());

            _timer_event.disable_all();
            Socket::close(_timer_fd);
        }

        void TimerQueue::cancel_all()
        {
            assert(_loop->in_loop_thread());

            for (auto &timer : _timer_set) {
                delete timer.second;
            }
            _timer_set.clear();
            _timer_map.clear();
            _canceling_timers.clear();
        }

        timespec TimerQueue::from_now(time_point when)
        {
            using namespace std::chrono;
            TimerQueue::duration du = when - system_clock::now();

            timespec ts{};
            ts.tv_sec = static_cast<time_t>(duration_cast<seconds>(du).count());

            ts.tv_nsec = static_cast<long>(duration_cast<nanoseconds>(du % seconds{1}).count());
            return ts;
        }

        void TimerQueue::read_timer(time_point now)
        {
            uint64_t howmany;
            ssize_t n = ::read(_timer_fd, &howmany, sizeof howmany);
            LOG_TRACE << "timeout:"<< howmany;
            if (n != sizeof howmany) {
                LOG_ERROR << "TimerQueue::handle_expire() reads " << n << " bytes instead of 8";
            }
        }

        void TimerQueue::reset_timer(time_point expiration)
        {
            struct itimerspec newValue{};
            struct itimerspec oldValue{};

            newValue.it_value = from_now(expiration);
            int ret = ::timerfd_settime(_timer_fd, 0, &newValue, &oldValue);
            if (ret) {
                LOG_ERROR << "timerfd_settime()";
            }
        }

        uint64_t TimerQueue::add_timer(const EventCallback &cb, time_point when, std::chrono::milliseconds interval)
        {
            auto *timer = new Timer(cb, when, interval);

            _loop->run_in_loop([this, timer] { add_timer_in_loop(timer); });
            return timer->sequence();
        }

        uint64_t
        TimerQueue::add_timer(EventCallback &&cb, TimerQueue::time_point when, std::chrono::milliseconds interval)
        {
            auto *timer = new Timer(std::move(cb), when, interval);

            _loop->run_in_loop([this, timer] { add_timer_in_loop(timer); });
            return timer->sequence();
        }

        void TimerQueue::cancel(uint64_t timerId)
        {
            _loop->run_in_loop([this, timerId] { cancel_in_loop(timerId); });
        }

        void TimerQueue::add_timer_in_loop(Timer *timer)
        {

            bool earliestChanged = insert(timer);

            if (earliestChanged) {
                reset_timer(timer->expiration());
            }
        }

        void TimerQueue::cancel_in_loop(uint64_t timerId)
        {

            assert(_timer_set.size() == _timer_map.size());

            auto it = _timer_map.find(timerId);
            if (it != _timer_map.end()) {
                _timer_set.erase(TimerNode(it->second->expiration(), it->second));
                delete it->second;
                _timer_map.erase(it);
            }
            else if (_calling_expired_timers) {
                _canceling_timers.insert(timerId);
            }

            assert(_timer_set.size() == _timer_map.size());
        }

        void TimerQueue::handle_expire()
        {
            time_point now_time(now());
            read_timer(now_time);

            std::vector<TimerNode> expired = get_expired(now_time);

            _calling_expired_timers = true;
            _canceling_timers.clear();

            for (auto &it : expired) {
                it.second->run();
            }
            _calling_expired_timers = false;

            reset(expired, now_time);
        }

        std::vector<TimerQueue::TimerNode> TimerQueue::get_expired(time_point now)
        {
            assert(_timer_set.size() == _timer_map.size());

            std::vector<TimerNode> expired;

            TimerNode sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));

            auto end = _timer_set.lower_bound(sentry);

            assert(end == _timer_set.end() || now < end->first);

            std::copy(_timer_set.begin(), end, back_inserter(expired));
            _timer_set.erase(_timer_set.begin(), end);

            for (auto &it : expired) {
                size_t n = _timer_map.erase(it.second->sequence());
                assert(n == 1);
            }

            assert(_timer_set.size() == _timer_map.size());
            return expired;
        }

        void TimerQueue::reset(const std::vector<TimerNode> &expired, time_point now)
        {
            time_point nextExpire;

            for (auto &it : expired) {
                if (it.second->repeat() && _canceling_timers.find(it.second->sequence()) == _canceling_timers.end()) {
                    it.second->restart(now);
                    insert(it.second);
                }
                else {
                    delete it.second;
                }
            }

            if (!_timer_set.empty()) {
                nextExpire = _timer_set.begin()->second->expiration();
            }

            if (nextExpire > time_point{}) {
                reset_timer(nextExpire);
            }
        }

        bool TimerQueue::insert(Timer *timer)
        {

            assert(_timer_set.size() == _timer_map.size());

            bool earliestChanged = false;

            time_point when = timer->expiration();

            auto it = _timer_set.begin();
            if (it == _timer_set.end() || when < it->first) {
                earliestChanged = true;
            }

            {
                std::pair<TimerSet::iterator, bool> result = _timer_set.insert(TimerNode(when, timer));
                assert(result.second);
                (void) result;
            }
            {
                std::pair<TimerMap::iterator, bool> result
                        = _timer_map.insert(std::pair<uint64_t, Timer *>(timer->sequence(), timer));
                assert(result.second);
                (void) result;
            }

            assert(_timer_set.size() == _timer_map.size());
            return earliestChanged;
        }

        TimerQueue::time_point TimerQueue::now()
        {
            return std::chrono::system_clock::now();
        }
    }
}
