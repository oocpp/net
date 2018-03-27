

#include"Timer.h"

namespace net {
    std::atomic<uint64_t> Timer::id{0};

    void Timer::restart(time_point now) {
      if (repeat_) {
        expiration_ = now+interval_;
      } else {
        expiration_ = time_point{};
      }
    }

}