//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <memory>
#include "Socket.h"
#include"InetAddress.h"
#include"Event.h"

namespace net{

    class TcpConnection;
    class InetAddress;
    class EventLoop;

    class Accepter {
    public:
        Accepter(EventLoop*loop,const InetAddress&addr);
        ~Accepter();

        void set_new_connection_cb(const std::function<void(int, const InetAddress &)> &cb){
            _new_connection_cb=cb;
        }

        void listen(int backlog=SOMAXCONN);
        void stop();

        void handle_accept();
    private:
        EventLoop*_loop;
        int _fd;
        InetAddress _addr;

        Event _event;
        std::function<void(int,const InetAddress&)>_new_connection_cb;
    };
}