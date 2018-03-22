//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <memory>
#include "Socket.h"
#include"InetAddress.h"
#include"Event.h"
#include"CallBack.h"

namespace net{

    class TcpConnection;
    class InetAddress;
    class EventLoop;

    class Accepter {
    public:
        using NewConnCallback = std::function<void(int, const InetAddress &)>;

        Accepter(EventLoop*loop,const InetAddress&addr);
        ~Accepter();

        void set_new_connection_cb(const NewConnCallback &cb){
            _new_connection_cb=cb;
        }

        void listen(int backlog=SOMAXCONN);
        void stop();

    private:

        void handle_accept();
    private:
        EventLoop*_loop;
        int _fd;
        InetAddress _addr;

        Event _event;
        NewConnCallback _new_connection_cb;
    };
}