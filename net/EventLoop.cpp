//
// Created by lg on 17-4-21.
//

#include "EventLoop.h"

#include "Log.h"
#include "EventLoopThread.h"
#include "Acceptor.h"
#include "Connector.h"

namespace net {
    EventLoop::EventLoop(){}


    EventLoop::~EventLoop() {

    }

    void EventLoop::add(std::unique_ptr<Accepter> &acc) {

    }

    void EventLoop::add(std::shared_ptr<TcpConnection> &conn) {

    }

    void EventLoop::erase(std::shared_ptr<TcpConnection> &acc) {

    }

    void EventLoop::erase(std::unique_ptr<Accepter> &) {

    }

    void EventLoop::run() {
        while(_is_looping){

        }
    }

    void EventLoop::stop() {
        _is_looping=false;
    }


}
