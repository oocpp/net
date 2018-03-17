//
// Created by lg on 18-3-16.
//

#include "Event.h"

namespace net {

    Event::Event(EventLoop *loop, int fd, bool watch_read, bool watch_write) noexcept
            : _loop(loop), _fd(fd) {

    }

    void Event::detachFromLoop() {

    }

    void Event::attachToLoop() {
        // assert(!IsNoneEvent());
        // assert(loop_->IsInLoopThread());
    /*
        if (attached_) {
            // FdChannel::Update may be called many times
            // So doing this can avoid event_add will be called more than once.
            DetachFromLoop();
        }

        assert(!attached_);
        ::event_set(event_, fd_, events_ | EV_PERSIST,
                    &FdChannel::HandleEvent, this);
        ::event_base_set(loop_->event_base(), event_);

        if (EventAdd(event_, nullptr) == 0) {
            DLOG_TRACE << "fd=" << fd_ << " watching event " << EventsToString();
            attached_ = true;
        } else {
            LOG_ERROR << "this=" << this << " fd=" << fd_ << " with event " << EventsToString() << " attach to event loop failed";
        }
        */
    }

}