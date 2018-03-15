//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <memory>

namespace net{

    class TcpConnection;
    class InetAddress;

    class Connector {
    public:
        Connector(const InetAddress&addr);

        void setConnectedCallBack(const std::function<void(std::unique_ptr<TcpConnection>&)>&cb);

        void setConnectedCallBack(std::function<void(std::unique_ptr<TcpConnection>&)>&&cb);

        std::unique_ptr<TcpConnection> getTcpSession();

    private:
        void connect(const InetAddress &peerAddr);
        //void connecting(int sockfd);
        //void retry(int sockfd);

    private:
        std::unique_ptr<TcpConnection> _tcp_ptr;
    };
}