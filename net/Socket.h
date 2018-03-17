//
// Created by lg on 17-4-18.
//

#pragma once

#include <sys/socket.h>
#include <cerrno>

namespace net {
    class InetAddress;

    namespace Socket
    {
        int create_nonblocking_socket(sa_family_t family = AF_INET);

        void bind(int sockfd,const InetAddress& localaddr);

        void listen(int sockfd,int backlog=SOMAXCONN);

        int connect(int sockfd,const InetAddress& localaddr);

        int accept(int sockfd,InetAddress &peeraddr);

        void close(int sockfd);

        int get_socket_error(int sockfd);

        bool get_tcp_info(struct tcp_info *) ;
        //bool getTcpInfoString(char* buf, int len) ;

        //getPeerAddr
        //getLocalAddr

        void shutdownWrite();

    };
}

