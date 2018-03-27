#pragma once

#include <atomic>
#include <chrono>
#include "CallBack.h"


namespace net {
    using  namespace std::literals::chrono_literals;

    class Timer {
    public:
        using time_point =std::chrono::system_clock::time_point;


        Timer(const TimerCallback &cb, time_point when, std::chrono::milliseconds interval)
                : callback_(cb),
                  expiration_(when),
                  interval_(interval),
                  repeat_(interval > 0ms),
                  sequence_(++id) {}


        void run() const {
          callback_();
        }

        time_point expiration() const { return expiration_; }

        bool repeat() const { return repeat_; }

        uint64_t sequence() const { return sequence_; }

        void restart(time_point now);

        bool isValid()const{return expiration_> time_point{};}

    private:
        TimerCallback callback_;
        time_point expiration_;
        std::chrono::milliseconds interval_;
        bool repeat_;

        const uint64_t sequence_;

        static std::atomic<uint64_t > id;
    };
}
