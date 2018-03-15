//
// Created by lg on 17-4-19.
//

#include "TcpConnection.h"
#include "Buffer.h"

namespace net{




    TcpConnection::TcpConnection(int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr)
    :_sockfd(sockfd)
    ,_local_addr(localAddr)
    ,_peer_addr(peerAddr){

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