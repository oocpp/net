#include <sys/socket.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <csignal>
#include <cassert>
#include "EventLoop.h"
#include "Log.h"
#include "EventLoopThread.h"
#include "Acceptor.h"
#include "Socket.h"
#include "Event.h"

namespace
{
    int createWakeEventfd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            LOG_ERROR << "Failed in eventfd";
            abort();
        }
        return evtfd;
    }

#pragma GCC diagnostic ignored "-Wold-style-cast"

    struct IgnoreSigPipe
    {
        IgnoreSigPipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
            //LOG_TRACE << "Ignore SIGPIPE";
        }
    };

#pragma GCC diagnostic error "-Wold-style-cast"

    IgnoreSigPipe initObj;
}

namespace net
{
    EventLoop::~EventLoop()noexcept
    {
        assert(!_is_looping);

        _wake_event.detach_from_loop();
        Socket::close(_wake_fd);

        assert(!_wake_event.is_add_to_loop());
    }

    EventLoop::EventLoop()noexcept
            : _is_looping(false)
              , _is_pending_fns(false)
              , _wake_fd(createWakeEventfd())
              , _th_id(std::this_thread::get_id())
              , _timers(this)
              , _wake_event(this, _wake_fd, true)
    {
        //_wake_event.set_read_cb(std::bind(&EventLoop::handle_wakeup_read, this));
        _wake_event.set_read_cb([this]{handle_wakeup_read();});
        _wake_event.attach_to_loop();
    }

    void EventLoop::run()
    {
        assert(!_is_looping);
        assert(_th_id == std::this_thread::get_id());
        LOG_TRACE;

        bool t = false;
        if (!_is_looping.compare_exchange_strong(t, true))
            return;

        while (_is_looping) {
            _poll.wait(-1, _events);

            for (auto &e:_events)
                reinterpret_cast<impl::Event *>(e.data.ptr)->handle_event(e.events);

            LOG_TRACE<<_events.size();
            do_pending_fn();
        }
        do_pending_fn();
        _timers.cancel_all();
        LOG_TRACE << " loop stop" << std::endl;
    }

    void EventLoop::stop()
    {
        LOG_TRACE;

        bool t = true;
        if (_is_looping.compare_exchange_strong(t, false)) {
            wakeup();
        }
    }

    void EventLoop::wakeup()
    {
        uint64_t one = 1;
        ssize_t n = ::write(_wake_fd, &one, sizeof one);
        if (n != sizeof one) {
            LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
        }
    }

    void EventLoop::handle_wakeup_read()
    {
        uint64_t one = 1;
        ssize_t n = ::read(_wake_fd, &one, sizeof one);
        if (n != sizeof one) {
            LOG_ERROR << "EventLoop::handle_expire() reads " << n << " bytes instead of 8";
        }
    }

    void EventLoop::add(impl::Event *e)
    {
        epoll_event event;
        event.events = e->get_events();
        event.data.ptr = e;
        _poll.add(e->get_fd(), event);
    }

    void EventLoop::update(impl::Event *e)
    {
        epoll_event event;
        event.events = e->get_events();
        event.data.ptr = e;
        _poll.update(e->get_fd(), event);
    }

    void EventLoop::remove(impl::Event *e)
    {
        _poll.remove(e->get_fd());
    }

    void EventLoop::run_in_loop(const EventCallback &cb)
    {
        if (in_loop_thread()) {
            cb();
        }
        else {
            queue_in_loop(cb);
        }
    }

    void EventLoop::run_in_loop(EventCallback &&cb)
    {
        if (in_loop_thread()) {
            cb();
        }
        else {
            queue_in_loop(std::move(cb));
        }
    }

    void EventLoop::queue_in_loop(EventCallback &&cb)
    {
        {
            std::lock_guard<std::mutex> l(_mu);
            _pending_fns.push_back(std::move(cb));
        }

        if (!in_loop_thread() || _is_pending_fns) {
            wakeup();
        }
    }

    bool EventLoop::in_loop_thread() const noexcept
    {
        static thread_local std::thread::id th_id = std::this_thread::get_id();
        return th_id == _th_id;
    }

    void EventLoop::queue_in_loop(const EventCallback &cb)
    {
        {
            std::lock_guard<std::mutex> l(_mu);
            _pending_fns.push_back(cb);
        }

        if (!in_loop_thread() || _is_pending_fns) {
            wakeup();
        }
    }

    void EventLoop::do_pending_fn()
    {
        std::vector<std::function<void()>> fns;
        _is_pending_fns = true;
        {
            std::lock_guard<std::mutex> l(_mu);
            fns.swap(_pending_fns);
        }

        for (auto &f:fns)
            f();
        _is_pending_fns = false;
    }

    uint64_t EventLoop::run_after(std::chrono::milliseconds ms, const EventCallback &cb)
    {
        return run_at(impl::TimerQueue::now() + ms, cb);
    }

    uint64_t EventLoop::run_at(impl::TimerQueue::time_point time, const EventCallback &cb)
    {
        return _timers.add_timer(cb, time, std::chrono::milliseconds{0});
    }

    uint64_t EventLoop::run_every(std::chrono::milliseconds ms, const EventCallback &cb)
    {
        return _timers.add_timer(cb, impl::TimerQueue::now() + ms, ms);
    }

    void EventLoop::cancel(uint64_t id)
    {
        _timers.cancel(id);
    }

    void EventLoop::reset_thread_id() noexcept
    {
        _th_id = std::this_thread::get_id();
    }

    uint64_t EventLoop::run_after(std::chrono::milliseconds ms, EventCallback &&cb)
    {
        return run_at(impl::TimerQueue::now() + ms, std::move(cb));
    }

    uint64_t EventLoop::run_at(impl::TimerQueue::time_point time, EventCallback &&cb)
    {
        return _timers.add_timer(std::move(cb), time, std::chrono::milliseconds{0});
    }

    uint64_t EventLoop::run_every(std::chrono::milliseconds ms, EventCallback &&cb)
    {
        return _timers.add_timer(std::move(cb), impl::TimerQueue::now() + ms, ms);
    }
}
