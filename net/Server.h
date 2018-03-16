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

namespace net{
    class EventLoop;
    class Accepter;
    class TcpConnection;

    class Server {
    public:
        Server(EventLoop*loop,const InetAddress& addr,const std::string &name,size_t threadSize=0);

        void init();
        void run();
        void stop();

        void handleNewConnection(int fd,const InetAddress &addr);

        using TCPConnPtr=std::shared_ptr<TcpConnection> ;
    private:
        EventLoop*_loop_ptr;
        InetAddress _addr;
        std::string _name;
        uint64_t _next_conn_id = 0;
        std::map<uint64_t,TCPConnPtr>_connections;

        EventLoopThreadPool _pool;
        std::shared_ptr<Accepter>_accepter;

        std::function<void()>_new_connection_cb;
        std::function<void()>_message_cb;
    };

}

