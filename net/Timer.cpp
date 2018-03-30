

#include"Timer.h"

namespace net {
    std::atomic<uint64_t> Timer::id{0};

    void Timer::restart(time_point now) {
      if (repeat_) {
        expiration_ = now+interval_;
      }
      else {
        expiration_ = time_point{};
      }
    }

    Timer::Timer(const TimerCallback &cb, Timer::time_point when, std::chrono::milliseconds interval)
            : callback_(cb),
            expiration_(when),
            interval_(interval),
            repeat_(interval > 0ms),
            sequence_(++id)
    {}


    Timer::Timer(TimerCallback &&cb, Timer::time_point when, std::chrono::milliseconds interval)
            : callback_(std::move(cb)),
            expiration_(when),
            interval_(interval),
            repeat_(interval > 0ms),
            sequence_(++id)
    {}

    void Timer::run() const {
        callback_();
    }

    Timer::time_point Timer::expiration() const noexcept {
        return expiration_;
    }

    bool Timer::repeat() const noexcept {
        return repeat_;
    }

    uint64_t Timer::sequence() const noexcept {
        return sequence_;
    }

    bool Timer::isValid() const noexcept {
        return expiration_> time_point{};
    }

}