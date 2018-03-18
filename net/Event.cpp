//
// Created by lg on 18-3-16.
//

#include "Event.h"
#include"EventLoop.h"
#include "Log.h"
#include<cassert>

namespace net {

    Event::Event(EventLoop *loop, int fd, bool r, bool w) noexcept
            : _loop(loop)
            , _fd(fd)
            ,_add_to_loop(false)
            //,_active_event(NoneEvent)
            ,_events((r ? ReadEvent : NoneEvent) | (w ? WriteEvent : NoneEvent))
            {

    }

    void Event::detach_from_loop() {
        if (_add_to_loop) {
            _loop->remove(this);
            _add_to_loop=false;
        }
    }

    void Event::attach_to_loop() {
        assert(!is_none());

        if (_add_to_loop) {
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
        else{
            attach_to_loop();
        }
    }

    void Event::enable_read() {
        if(!is_read()){
            _events |= ReadEvent;
            update();
        }
    }

    void Event::enable_write() {
        if(!is_write()) {
            _events |= WriteEvent;
            update();
        }
    }

    void Event::enable_all() {
        if(!is_write()||!is_read()) {
            _events |= ReadEvent|WriteEvent;
            update();
        }
    }

    void Event::disable_read() {
        if(is_read()) {
            _events &= ~ReadEvent;
            update();
        }
    }

    void Event::disable_write() {
        if(is_write()) {
            _events &= ~WriteEvent;
            update();
        }
    }

    void Event::disable_all() {
        if(is_write()||is_read()) {
            _events = NoneEvent;
            detach_from_loop();
        }
    }

    void Event::handle_event(uint32_t event) {
        if ((event & ReadEvent) && _read_cb) {
            _read_cb();
        }

        if ((event & WriteEvent) && _write_cb) {
            _write_cb();
        }
    }

    Event::~Event() noexcept {
        detach_from_loop();
    }


}