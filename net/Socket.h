#pragma once

#include <sys/socket.h>
#include <cerrno>
#include <netinet/in.h>

namespace net
{
    class InetAddress;

    namespace Socket
    {
        int create_nonblocking_socket(sa_family_t family = AF_INET);

        void bind(int sockfd, const InetAddress &localaddr);

        void listen(int sockfd, int backlog = SOMAXCONN);

        int connect(int sockfd, const InetAddress &localaddr);

        int accept(int sockfd, InetAddress &peeraddr);

        void close(int sockfd);

        int get_socket_error(int sockfd);

        bool get_tcp_info(struct tcp_info *);
        //bool getTcpInfoString(char* buf, int len) ;

        bool is_self_connect(int sockfd);

        sockaddr_in get_peer_addr(int fd);

        sockaddr_in get_local_addr(int fd);

        void shutdownWrite(int fd);

        void SetKeepAlive(int fd, bool on);

        void setTcpNoDelay(int fd, bool on);

        void setNonBlockAndCloseOnExec(int sockfd);
    };
}
