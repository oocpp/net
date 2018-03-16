//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <memory>
#include "Socket.h"
#include"InetAddress.h"


namespace net{

    class TcpConnection;
    class InetAddress;

    class Accepter {
    public:
        Accepter(const InetAddress&addr);
        ~Accepter();

        void setNewConnectedCallBack(const std::function<void(int,InetAddress)>&cb){
            _new_connection_cb=cb;
        }

        void listen(int backlog=SOMAXCONN);
        void accept();
        void stop();
    private:
        int _fd;
        InetAddress _addr;
        std::function<void(int,InetAddress)>_new_connection_cb;
    };
}