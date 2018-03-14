//
// Created by lg on 17-4-19.
//

#include "TcpConnection.h"
#include "Buffer.h"

namespace net{

    EventBase &TcpSession::getEventBase() {
        //return static_cast<EventBase&>(*this);
    }


    TcpSession::TcpSession(int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr)
    :_sockfd(sockfd)
    ,_local_addr(localAddr)
    ,_peer_addr(peerAddr){

    }

    TcpSession::TcpSession(const InetAddress &peerAddr)
    :_sockfd(peerAddr.getfamily())
    ,_peer_addr(peerAddr)
    {

    }

    void TcpSession::close() {
        if(_tcp_state==TcpState::GOOD)
            _tcp_state=TcpState::CLOSING;
    }
}