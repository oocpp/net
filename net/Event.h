#pragma once

#include <sys/epoll.h>
#include"CallBack.h"

namespace net
{
    class EventLoop;

    namespace impl
    {
        class Event
        {
        public:
            explicit Event(EventLoop *loop, int fd = -1, bool r = false, bool w = false)noexcept;

            ~Event()noexcept;

            Event(const Event &) = delete;

            Event &operator==(const Event &)= delete;

            Event(Event &&e)noexcept;

            static constexpr uint32_t NoneEvent = 0;
            static constexpr uint32_t ReadEvent = EPOLLIN | EPOLLPRI;
            static constexpr uint32_t WriteEvent = EPOLLOUT;
            static constexpr uint32_t CloseEvent = EPOLLRDHUP;

            void set_read_cb(const EventCallback &cb);

            void set_write_cb(const EventCallback &cb);

            void set_close_cb(const EventCallback &cb);

            void set_read_cb(EventCallback &&cb)noexcept;

            void set_write_cb(EventCallback &&cb)noexcept;

            void set_close_cb(EventCallback &&cb)noexcept;

            /// enable系列函数会使event加入到事件循环里面

            void enable_read();

            void enable_write();

            void enable_all();

            /// disable系列函数，如果导致事件类型为NoneEvent，
            /// 则事件会从事件循环里面删除。

            void disable_read();

            void disable_write();

            /// 从事件循环里删除
            /// 重置已设置的事件类型
            void disable_all();

            void set_fd(int fd)noexcept;

            int get_fd() const noexcept;

            uint32_t get_events() const noexcept;

            void attach_to_loop();

            /// 取消对事件的检测
            /// 不会重置已设置的事件类型
            void detach_from_loop();

            void async_enable_read();

            void async_enable_write();

            void async_enable_all();

            void async_disable_read();

            void async_disable_write();

            /// 从事件循环里删除
            /// 重置已设置的事件类型
            void async_disable_all();

            void async_attach_to_loop();

            void async_detach_from_loop();

            bool is_add_to_loop() const noexcept;

            bool is_writable() const noexcept;

            bool is_readable() const noexcept;

            bool is_none() const noexcept;

        public:
            /// 用户不应该调用
            void handle_event(uint32_t event);

        private:
            void update();

        private:
            EventLoop *_loop;
            int _fd;
            bool _add_to_loop;
            uint32_t _events;

            EventCallback _read_cb;
            EventCallback _write_cb;
            EventCallback _close_cb;
        };
    }
}
