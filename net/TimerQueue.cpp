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
            _timer_event.disable_all();
            Socket::close(_timer_fd);

            for (auto &timer : _timers) {
                delete timer.second;
            }
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
            LOG_TRACE << howmany;
            if (n != sizeof howmany) {
                LOG_ERROR << "TimerQueue::handle_expire() reads " << n << " bytes instead of 8";
            }
        }

        void TimerQueue::reset_timer(time_point expiration)
        {
            struct itimerspec newValue{};
            struct itimerspec oldValue{};

            memset(&newValue, 0, sizeof newValue);
            memset(&oldValue, 0, sizeof oldValue);

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

            assert(_timers.size() == _active_timers.size());

            auto it = _active_timers.find(timerId);
            if (it != _active_timers.end()) {
                _timers.erase(TimerNode(it->second->expiration(), it->second));

                _active_timers.erase(it);
            }
            else if (_calling_expired_timers) {
                _canceling_timers.insert(timerId);
            }

            assert(_timers.size() == _active_timers.size());
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
            assert(_timers.size() == _active_timers.size());

            std::vector<TimerNode> expired;

            TimerNode sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));

            auto end = _timers.lower_bound(sentry);

            assert(end == _timers.end() || now < end->first);

            std::copy(_timers.begin(), end, back_inserter(expired));
            _timers.erase(_timers.begin(), end);

            for (auto &it : expired) {
                size_t n = _active_timers.erase(it.second->sequence());
                assert(n == 1);
            }

            assert(_timers.size() == _active_timers.size());
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

            if (!_timers.empty()) {
                nextExpire = _timers.begin()->second->expiration();
            }

            if (nextExpire > time_point{}) {
                reset_timer(nextExpire);
            }
        }

        bool TimerQueue::insert(Timer *timer)
        {

            assert(_timers.size() == _active_timers.size());

            bool earliestChanged = false;

            time_point when = timer->expiration();

            auto it = _timers.begin();
            if (it == _timers.end() || when < it->first) {
                earliestChanged = true;
            }

            {
                std::pair<TimerList::iterator, bool> result = _timers.insert(TimerNode(when, timer));
                assert(result.second);
                (void) result;
            }
            {
                std::pair<ActiveTimerMap::iterator, bool> result
                        = _active_timers.insert(std::pair<uint64_t, Timer *>(timer->sequence(), timer));
                assert(result.second);
                (void) result;
            }

            assert(_timers.size() == _active_timers.size());
            return earliestChanged;
        }

        TimerQueue::time_point TimerQueue::now()
        {
            return std::chrono::system_clock::now();
        }
    }
}
