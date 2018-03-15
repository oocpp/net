//
// Created by lg on 18-3-13.
//

#pragma once

#include"EventLoopThreadPool.h"
#include "InetAddress.h"
#include<memory>
#include<string>
#include<functional>

namespace net{
    class EventLoop;
    class Accepter;

    class Server {
    public:
        Server(EventLoop*loop,const InetAddress& addr,const std::string &name,size_t threadSize=0);

        void init();
        void run();
        void stop();
    private:
        EventLoop*_loop_ptr;
        InetAddress _addr;
        std::string _name;

        EventLoopThreadPool _pool;
        std::shared_ptr<Accepter>_accepter;

        std::function<void()>_new_connection_cb;
        std::function<void()>_message_cb;
    };

}

