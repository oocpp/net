#pragma once

#include <atomic>
#include <chrono>
#include "CallBack.h"

namespace net
{
    namespace impl
    {
        class Timer
        {
        public:
            using time_point =std::chrono::system_clock::time_point;

            Timer(const EventCallback &cb, time_point when, std::chrono::milliseconds interval);

            Timer(EventCallback &&cb, time_point when, std::chrono::milliseconds interval);

            void run() const;

            time_point expiration() const noexcept;

            bool repeat() const noexcept;

            uint64_t sequence() const noexcept;

            void restart(time_point now);

            bool is_valid() const noexcept;

        private:
            EventCallback _callback;
            time_point _expiration;
            std::chrono::milliseconds _interval;
            bool _repeat;

            const uint64_t _sequence;

            static std::atomic<uint64_t> id;
        };
    }
}
