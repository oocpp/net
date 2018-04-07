#pragma once

#include<functional>
#include <sys/epoll.h>
#include"CallBack.h"

namespace net
{
    class EventLoop;

    class Event
    {
    public:
        Event(EventLoop *loop, int fd, bool r = false, bool w = false)noexcept;

        ~Event()noexcept;
        Event(const Event&)=delete;
        Event &operator==(const Event &)= delete;

        Event(Event&&e)noexcept ;

        static constexpr uint32_t NoneEvent = 0;
        static constexpr uint32_t ReadEvent = EPOLLIN | EPOLLPRI;
        static constexpr uint32_t WriteEvent = EPOLLOUT;

        void set_read_cb(const ReadEventCallback &cb);

        void set_write_cb(const EventCallback &cb);

        void set_error_cb(const EventCallback &cb);

        void set_read_cb(ReadEventCallback &&cb)noexcept;

        void set_write_cb(EventCallback &&cb)noexcept;

        void set_error_cb(EventCallback &&cb)noexcept;

        void enable_read();

        void enable_write();

        void enable_all();

        void disable_read();

        void disable_write();

        void disable_all();

        int get_fd() const noexcept;

        uint32_t get_events() const noexcept;

        void attach_to_loop();

        void detach_from_loop();

        void handle_event(uint32_t event);

        bool is_add_to_loop() const noexcept;

        void set_fd(int fd)noexcept;

        bool is_write() const noexcept;

        bool is_read() const noexcept;

        bool is_none() const noexcept;

    private:
        void update();

    private:
        EventLoop *_loop;
        int _fd;
        bool _add_to_loop;
        uint32_t _events;

        ReadEventCallback _read_cb;
        EventCallback _write_cb;
        EventCallback _error_cb;
    };
}

