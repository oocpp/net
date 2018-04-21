#include"Timer.h"

namespace net
{
    namespace impl
    {
        std::atomic<uint64_t> Timer::id{0};

        void Timer::restart(time_point now)
        {
            if (_repeat) {
                _expiration = now + _interval;
            }
            else {
                _expiration = time_point{};
            }
        }

        Timer::Timer(const EventCallback &cb, Timer::time_point when, std::chrono::milliseconds interval)
                : _callback(cb)
                  , _expiration(when)
                  , _interval(interval)
                  , _repeat(interval > std::chrono::milliseconds{0})
                  , _sequence(++id)
        {}


        Timer::Timer(EventCallback &&cb, Timer::time_point when, std::chrono::milliseconds interval)
                : _callback(std::move(cb))
                  , _expiration(when)
                  , _interval(interval)
                  , _repeat(interval > std::chrono::milliseconds{0})
                  , _sequence(++id)
        {}

        void Timer::run() const
        {
            _callback();
        }

        Timer::time_point Timer::expiration() const noexcept
        {
            return _expiration;
        }

        bool Timer::repeat() const noexcept
        {
            return _repeat;
        }

        uint64_t Timer::sequence() const noexcept
        {
            return _sequence;
        }

        bool Timer::is_valid() const noexcept
        {
            return _expiration > time_point{};
        }
    }
}
