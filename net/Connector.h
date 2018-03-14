//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <memory>

namespace net{

    class TcpSession;
    class InetAddress;

    class Connector {
    public:
        Connector(const InetAddress&addr);

        void setConnectedCallBack(const std::function<void(std::unique_ptr<TcpSession>&)>&cb);

        void setConnectedCallBack(std::function<void(std::unique_ptr<TcpSession>&)>&&cb);

        std::unique_ptr<TcpSession> getTcpSession();

    private:
        void connect(const InetAddress &peerAddr);
        //void connecting(int sockfd);
        //void retry(int sockfd);

    private:
        std::unique_ptr<TcpSession> _tcp_ptr;
    };
}