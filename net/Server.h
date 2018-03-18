//
// Created by lg on 18-3-13.
//

#pragma once

#include"EventLoopThreadPool.h"
#include "InetAddress.h"
#include<memory>
#include<string>
#include<functional>
#include<map>
#include"Acceptor.h"
#include"CallBack.h"

namespace net{
    class EventLoop;
    class TcpConnection;

    class Server {
    public:
        Server(EventLoop*loop,const InetAddress& addr,const std::string &name,size_t threadSize=0);

        void run();
        void stop();

        void handle_new_connection(int fd, const InetAddress &addr);


    private:
        EventLoop*_loop_ptr;
        EventLoopThreadPool _pool;
        InetAddress _addr;
        std::string _name;
        Accepter _accepter;

        uint64_t _next_conn_id = 0;
        std::map<uint64_t,TCPConnPtr>_connections;


        std::function<void()>_new_connection_cb;
        MessageCallback _message_cb;
    };

}

