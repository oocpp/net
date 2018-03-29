//
// Created by lg on 17-4-18.
//


#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include "Log.h"
#include "Socket.h"
#include "InetAddress.h"

namespace net {
    namespace Socket {

        void SetKeepAlive(int fd, bool on) {
            int optval = on ? 1 : 0;
            int rc = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                                  reinterpret_cast<const char *>(&optval), static_cast<socklen_t>(sizeof optval));
            if (rc != 0) {
                int serrno = errno;
                LOG_ERROR << "setsockopt(SO_KEEPALIVE) failed, errno=" << serrno << " " << strerror(serrno);
            }
        }

        void SetReuseAddr(int fd) {
            int optval = 1;
            int rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                                  reinterpret_cast<const char *>(&optval), static_cast<socklen_t>(sizeof optval));
            if (rc != 0) {
                int serrno = errno;
                LOG_ERROR << "setsockopt(SO_REUSEADDR) failed, errno=" << serrno << " " << strerror(serrno);
            }
        }

        void SetReusePort(int fd) {
            int optval = 1;
            int rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT,
                                  reinterpret_cast<const char *>(&optval), static_cast<socklen_t>(sizeof optval));
            //LOG_INFO << "setsockopt SO_REUSEPORT fd=" << get_fd << " rc=" << rc;
            if (rc != 0) {
                int serrno = errno;
                LOG_ERROR << "setsockopt(SO_REUSEPORT) failed, errno=" << serrno << " " << strerror(serrno);
            }
        }

        void setTcpNoDelay(int fd,bool on)
        {
            int optval = on ? 1 : 0;
            int rc=::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                         &optval, static_cast<socklen_t>(sizeof optval));
            if (rc != 0) {
                int serrno = errno;
                LOG_ERROR << "setsockopt(TCP_NODELAY) failed, errno=" << serrno << " " << strerror(serrno);
            }
        }

        void bind(int sockfd, const InetAddress &addr) {
            if (::bind(sockfd, addr.get_sockaddr(), addr.get_sockaddr_size()) < 0) {
                LOG_ERROR << "bind 失败";
            }
        }

        void listen(int sockfd, int backlog) {
            if (::listen(sockfd, backlog) < 0) {
                LOG_ERROR << "listen 失败";
            }
        }

        int connect(int sockfd, const InetAddress &peeraddr) {
            return ::connect(sockfd, peeraddr.get_sockaddr(), peeraddr.get_sockaddr_size());
        }

        void close(int sockfd) {
            ::close(sockfd);
            LOG_INFO<<"fd= "<<sockfd<<"close";
        }

        int accept(int sockfd, InetAddress &peeraddr) {
            socklen_t addrlen = peeraddr.get_sockaddr_size();

            int connfd = ::accept4(sockfd, peeraddr.get_sockaddr(),
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
            } else {
                SetKeepAlive(connfd, true);
            }
            return connfd;
        }

        void shutdownWrite(int fd) {
            if (::shutdown(fd, SHUT_WR) < 0) {
                LOG_ERROR << "sockets::shutdownWrite";
            }
        }

        int create_nonblocking_socket(sa_family_t family) {

            int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

            if (sockfd < 0) {
                LOG_ERROR << "sockets::createNonblockingOrDie";
            }

            SetKeepAlive(sockfd, true);
            SetReuseAddr(sockfd);
            SetReusePort(sockfd);
            return sockfd;
        }

        void setNonBlockAndCloseOnExec(int sockfd) {
            int flags = ::fcntl(sockfd, F_GETFL, 0);
            flags |= O_NONBLOCK | FD_CLOEXEC;
            ::fcntl(sockfd, F_SETFL, flags);
        }

        int get_socket_error(int sockfd)
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

        sockaddr_in get_peer_addr(int fd) {
            struct sockaddr_in peeraddr;
            bzero(&peeraddr, sizeof peeraddr);
            socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
            if (::getpeername(fd, reinterpret_cast< sockaddr*>(&peeraddr), &addrlen) < 0)
            {
                LOG_ERROR << "sockets::getPeerAddr";
            }
            return peeraddr;
        }

        sockaddr_in get_local_addr(int fd) {
            sockaddr_in localaddr;
            bzero(&localaddr, sizeof localaddr);
            socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
            if (::getsockname(fd, reinterpret_cast< sockaddr*>(&localaddr), &addrlen) < 0)
            {
                LOG_ERROR << "sockets::getLocalAddr";
            }
            return localaddr;
        }

        bool is_self_connect(int sockfd)
        {
            struct sockaddr_in laddr4 = get_local_addr(sockfd);
            struct sockaddr_in raddr4 = get_peer_addr(sockfd);
            if (laddr4.sin_family == AF_INET)
            {
                return laddr4.sin_port == raddr4.sin_port
                       && laddr4.sin_addr.s_addr == raddr4.sin_addr.s_addr;
            }
            else
            {
                return false;
            }
        }
    }
}




