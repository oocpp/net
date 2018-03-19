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

    class TcpServer {
    public:
        TcpServer(EventLoop*loop,const InetAddress& addr,const std::string &name,size_t threadSize=0);

        void run();
        void stop();

        void handle_new_connection(int fd, const InetAddress &addr);

        void remove_connection(const TCPConnPtr& conn);
        void remove_connection_in_loop(const TCPConnPtr& conn);
    private:
        EventLoop*_loop;
        EventLoopThreadPool _pool;
        InetAddress _addr;
        std::string _name;
        Accepter _accepter;

        uint64_t _next_conn_id = 0;
        std::map<uint64_t,TCPConnPtr>_connections;


        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
    };

}

