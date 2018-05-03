#include <cassert>
#include "Event.h"
#include "EventLoop.h"
#include "Log.h"

namespace net {
    namespace impl {
        Event::Event(EventLoop *loop, int fd, bool r, bool w) noexcept
                : _loop(loop), _fd(fd), _add_to_loop(false)
                , _events((r ? ReadEvent : NoneEvent) | (w ? WriteEvent : NoneEvent)) {

        }

        Event::Event(Event &&e) noexcept
                : _loop(e._loop), _fd(e._fd)
                , _add_to_loop(e._add_to_loop)
                , _events(e._events)
                , _read_cb(std::move(e._read_cb))
                , _write_cb(std::move(e._write_cb))
                , _close_cb(std::move(e._close_cb))
        {
            e._loop = nullptr;
            e._fd = -1;
            e._add_to_loop = false;
        }

        void Event::detach_from_loop() {
            if (is_add_to_loop()) {
                _loop->remove(this);
                _add_to_loop = false;
            }
        }

        void Event::attach_to_loop() {
            assert(!is_none());

            if (is_add_to_loop()) {
                _loop->update(this);
            }
            else {
                _loop->add(this);
                _add_to_loop = true;
            }
        }

        void Event::update() {
            if (is_none())
                detach_from_loop();
            else {
                attach_to_loop();
            }
        }

        void Event::enable_read() {
            if (!is_readable()) {
                _events |= ReadEvent;
                update();
            }
        }

        void Event::enable_write() {
            if (!is_writable()) {
                _events |= WriteEvent;
                update();
            }
        }

        void Event::enable_all() {
            if (!is_writable() || !is_readable()) {
                _events |= ReadEvent | WriteEvent;
                update();
            }
        }

        void Event::disable_read() {
            if (is_readable()) {
                _events &= ~ReadEvent;
                update();
            }
        }

        void Event::disable_write() {
            if (is_writable()) {
                _events &= ~WriteEvent;
                update();
            }
        }

        void Event::disable_all() {
            if (is_writable() || is_readable()) {
                _events = NoneEvent;
                detach_from_loop();
            }
        }

        void Event::handle_event(uint32_t event) {
            if ((event & CloseEvent) && _close_cb) {
                _close_cb();
                return;
            }

            if ((event & ReadEvent) && _read_cb) {
                _read_cb();
            }

            if ((event & WriteEvent) && _write_cb) {
                _write_cb();
            }
        }

        Event::~Event() noexcept {
            assert(!is_add_to_loop());
        }

        void Event::set_read_cb(const EventCallback &cb) {
            _read_cb = cb;
        }

        void Event::set_write_cb(const EventCallback &cb) {
            _write_cb = cb;
        }

        void Event::set_close_cb(const EventCallback &cb) {
            _close_cb = cb;
        }

        void Event::set_read_cb(EventCallback &&cb) noexcept {
            _read_cb = std::move(cb);
        }

        void Event::set_write_cb(EventCallback &&cb) noexcept {
            _write_cb = std::move(cb);
        }

        void Event::set_close_cb(EventCallback &&cb)noexcept {
            _close_cb = std::move(cb);
        }

        int Event::get_fd() const noexcept {
            return _fd;
        }

        uint32_t Event::get_events() const noexcept {
            return _events;
        }

        bool Event::is_add_to_loop() const noexcept {
            return _add_to_loop;
        }

        void Event::set_fd(int fd) noexcept {
            _fd = fd;
        }

        bool Event::is_writable() const noexcept {
            return (_events & WriteEvent) != 0;
        }

        bool Event::is_readable() const noexcept {
            return (_events & ReadEvent) != 0;
        }

        bool Event::is_none() const noexcept {
            return _events == NoneEvent;
        }

        void Event::async_enable_read() {
            _loop->run_in_loop([this] { enable_read(); });
        }

        void Event::async_enable_write() {
            _loop->run_in_loop([this] { enable_write(); });
        }

        void Event::async_enable_all() {
            _loop->run_in_loop([this] { enable_all(); });
        }

        void Event::async_disable_read() {
            _loop->run_in_loop([this] { disable_read(); });
        }

        void Event::async_disable_write() {
            _loop->run_in_loop([this] { disable_write(); });
        }

        void Event::async_disable_all() {
            _loop->run_in_loop([this] { disable_all(); });
        }

        void Event::async_attach_to_loop() {
            _loop->run_in_loop([this] { attach_to_loop(); });
        }

        void Event::async_detach_from_loop() {
            _loop->run_in_loop([this] { detach_from_loop(); });
        }
    }
}
