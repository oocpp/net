#pragma once

#include <atomic>
#include <chrono>
#include "CallBack.h"


namespace net
{
    using namespace std::literals::chrono_literals;

    class Timer
    {
    public:
        using time_point =std::chrono::system_clock::time_point;

        Timer(const TimerCallback &cb, time_point when, std::chrono::milliseconds interval);

        Timer(TimerCallback &&cb, time_point when, std::chrono::milliseconds interval);

        void run() const;

        time_point expiration() const noexcept;

        bool repeat() const noexcept;

        uint64_t sequence() const noexcept;

        void restart(time_point now);

        bool isValid() const noexcept;

    private:
        TimerCallback callback_;
        time_point expiration_;
        std::chrono::milliseconds interval_;
        bool repeat_;

        const uint64_t sequence_;

        static std::atomic<uint64_t> id;
    };
}
