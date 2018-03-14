//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <memory>


namespace net{

    class TcpSession;
    class InetAddress;

    class Accepter {
    public:
        Accepter(const InetAddress&addr);

        void setNewConnectedCallBack(std::function<void(TcpSession&)>&cb){

        }
        void setNewConnectedCallBack(std::function<void(TcpSession&)>&&cb){
            std::move(cb);
        }

        std::unique_ptr<TcpSession>  getTcpSession();
    private:
        std::unique_ptr<TcpSession>  _tcp_ptr;
    };
}