//
// Created by lg on 17-4-18.
//

#pragma once

#include <sys/socket.h>
#include <cerrno>

namespace net {
    class InetAddress;

    class Socket final
    {
    private:

    public:
        explicit Socket(int _sockfd)
                : _sockfd(_sockfd)
        {}

        explicit Socket(sa_family_t family=AF_INET);

        ~Socket();
        int getSockFd() const { return _sockfd; }


        static int createNonblocking(sa_family_t family);
        static void setNonBlockAndCloseOnExec(int sockfd);


        static void bind(int sockfd,const InetAddress& localaddr);

        static  void listen(int sockfd);

        static int connect(int sockfd,const InetAddress& localaddr);

        static int accept(int sockfd,InetAddress &peeraddr);

        static void close(int sockfd);

        static int getSocketError(int sockfd)
        {
            int optval;
            socklen_t optlen = static_cast<socklen_t>(sizeof optval);

            if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
            {
                return errno;
            }
            else
            {
                return optval;
            }
        }

        bool getTcpInfo(struct tcp_info*) const;
        bool getTcpInfoString(char* buf, int len) const;

        //getPeerAddr
        //getLocalAddr

        void shutdownWrite();

    private:
        int _sockfd;
    };
}

