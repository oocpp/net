//
// Created by lg on 18-3-16.
//

#include "Event.h"
#include"EventLoop.h"

namespace net {

    Event::Event(EventLoop *loop, int fd, bool watch_read, bool watch_write) noexcept
            : _loop(loop), _fd(fd) {

    }

    void Event::detach_from_loop() {
        _loop->remove(this);
    }

    void Event::attach_to_loop() {
        _loop->add(this);
    }

}