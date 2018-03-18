//
// Created by lg on 17-4-19.
//

#include "TcpConnection.h"
#include "Buffer.h"
#include"Log.h"

namespace net{




    TcpConnection::TcpConnection(uint64_t id,EventLoop*loop,int sockfd,const InetAddress&local_addr,const InetAddress&peer_add)
    :_sockfd(sockfd)
     ,_id(id)
     ,_loop(loop)
    ,_local_addr(local_addr)
    ,_peer_addr(peer_add){

    }

    void TcpConnection::close() {
        if(_tcp_state==TcpState::GOOD)
            _tcp_state=TcpState::CLOSING;
    }

    void TcpConnection::attach_to_loop() {
        LOG_INFO<<"新链接:id="<<_id<<"  port"<<_peer_addr.get_port();
    }
}