//
// Created by lg on 17-4-19.
//

#include "TcpConnection.h"
#include "Buffer.h"

namespace net{




    TcpConnection::TcpConnection(uint64_t id,EventLoop*loop,int sockfd, const InetAddress &addr)
    :_sockfd(sockfd)
     ,_id(id)
     ,_loop(loop)
    ,_local_addr(addr) {

    }

    TcpConnection::TcpConnection(const InetAddress &peerAddr)
    :_sockfd(peerAddr.getfamily())
    ,_peer_addr(peerAddr)
    {

    }

    void TcpConnection::close() {
        if(_tcp_state==TcpState::GOOD)
            _tcp_state=TcpState::CLOSING;
    }
}