//
// Created by lg on 17-4-19.
//

#include "Connector.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Log.h"

namespace net {

    Connector::Connector(const InetAddress &peerAddr)
    {
        connect(peerAddr);
    }

    std::unique_ptr<TcpConnection> net::Connector::getTcpSession() {
       return std::move(_tcp_ptr);
    }

    void Connector::setConnectedCallBack(const std::function<void(std::unique_ptr<TcpConnection> &)> &cb) {

    }

    void Connector::setConnectedCallBack(std::function<void(std::unique_ptr<TcpConnection> &)> &&cb) {
       // _tcp_ptr->set_write_cb(std::move(cb));
    }

    void Connector::connect(const InetAddress &peerAddr) {

        int ret;// = Socket::connect(_tcp_ptr->getSockFd(), peerAddr);
        int savedErrno = (ret == 0) ? 0 : errno;

        switch (savedErrno)
        {
            case 0:
            case EINPROGRESS:
            case EINTR:
            case EISCONN:
                //connecting(sockfd);
                break;

            case EAGAIN:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case ECONNREFUSED:
            case ENETUNREACH:
                //retry(sockfd);
                //Socket::close(sockfd);
                break;

            case EACCES:
            case EPERM:
            case EAFNOSUPPORT:
            case EALREADY:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:
                LOG_ERROR << "connect error in Connector::startInLoop " << savedErrno;
                //Socket::close(sockfd);
                break;

            default:
                LOG_ERROR << "Unexpected error in Connector::startInLoop " << savedErrno;
                //Socket::close(sockfd);
                break;
        }
    }

}