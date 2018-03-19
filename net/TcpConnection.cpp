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
     ,_event(loop,sockfd)
    ,_local_addr(local_addr)
    ,_peer_addr(peer_add){

    }

    void TcpConnection::close() {

    }

    void TcpConnection::attach_to_loop() {
        _conn_status = Connected;
        _event.enable_read();

        if (_connecting_cb) {
            _connecting_cb(shared_from_this());
        }
    }
}