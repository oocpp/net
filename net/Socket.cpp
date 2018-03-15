//
// Created by lg on 17-4-18.
//



#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <netinet/tcp.h> // for TCP_NODELAY
#include <netinet/in.h>
#include "Log.h"
#include "Socket.h"
#include "InetAddress.h"

namespace net {

    void SetKeepAlive(int fd, bool on) {
        int optval = on ? 1 : 0;
        int rc = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                              reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
        if (rc != 0) {
            int serrno = errno;
            LOG_ERROR << "setsockopt(SO_KEEPALIVE) failed, errno=" << serrno << " " << strerror(serrno);
        }
    }

    void SetReuseAddr(int fd) {
        int optval = 1;
        int rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                              reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
        if (rc != 0) {
            int serrno = errno;
            LOG_ERROR << "setsockopt(SO_REUSEADDR) failed, errno=" << serrno << " " << strerror(serrno);
        }
    }

    void SetReusePort(int fd) {
        int optval = 1;
        int rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT,
                              reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
        LOG_INFO << "setsockopt SO_REUSEPORT fd=" << fd << " rc=" << rc;
        if (rc != 0) {
            int serrno = errno;
            LOG_ERROR << "setsockopt(SO_REUSEPORT) failed, errno=" << serrno << " " << strerror(serrno);
        }
    }

    Socket::~Socket() {
        if (::close(_sockfd) < 0) {
            LOG_TRACE << "sockets::close";
        }
    }

   Socket::Socket(sa_family_t family)
            : _sockfd(createNonblocking(family)) {

    }

    void Socket::bind(int sockfd, const InetAddress &addr) {
        if (::bind(sockfd, addr.getSockAddr(), addr.size()) < 0) {
            LOG_ERROR << "bind 失败";
        }
    }

    void Socket::listen(int sockfd) {
        if (::listen(sockfd, SOMAXCONN) < 0) {
            LOG_ERROR << "listen 失败";
        }
    }

    int Socket::connect(int sockfd, const InetAddress &peeraddr) {
        return ::connect(sockfd, peeraddr.getSockAddr(), peeraddr.size());
    }

    void Socket::close(int sockfd) {
        ::close(sockfd);
    }

    int Socket::accept(int sockfd, InetAddress &peeraddr) {
        socklen_t addrlen = peeraddr.size();

        int connfd = ::accept4(sockfd, peeraddr.getSockAddr(),
                               &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (connfd < 0) {

            int savedErrno = errno;

            switch (savedErrno) {
                case EAGAIN:
                case ECONNABORTED:
                case EINTR:
                case EPROTO: // ???
                case EPERM:
                case EMFILE: // per-process lmit of open file desctiptor ???
                    // expected errors
                    errno = savedErrno;
                    break;
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENFILE:
                case ENOBUFS:
                case ENOMEM:
                case ENOTSOCK:
                case EOPNOTSUPP:
                    // unexpected errors
                    LOG_ERROR << "unexpected error of ::accept " << savedErrno;
                    break;
                default:
                    LOG_ERROR << "unknown error of ::accept " << savedErrno;
                    break;
            }
        }
        return connfd;
    }

    void Socket::shutdownWrite() {
        if (::shutdown(_sockfd, SHUT_WR) < 0){
            LOG_ERROR << "sockets::shutdownWrite";
        }
    }


    int Socket::createNonblocking(sa_family_t family) {

        int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

        if (sockfd < 0) {
            LOG_ERROR << "sockets::createNonblockingOrDie";
        }


        SetKeepAlive(sockfd,true);
        SetReuseAddr(sockfd);
        SetReusePort(sockfd);
        return sockfd;
    }

     void Socket::setNonBlockAndCloseOnExec(int sockfd) {
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_NONBLOCK|FD_CLOEXEC;
        int ret = ::fcntl(sockfd, F_SETFL, flags);
        // FIXME
    }

}




