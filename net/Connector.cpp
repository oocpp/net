//
// Created by lg on 17-4-19.
//

#include "Connector.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Log.h"

namespace net {

    Connector::Connector(EventLoop *loop, const InetAddress &addr)
    :_loop(loop)
    ,_addr(addr)
    ,_status(Disconnected)
    ,_event(loop,-1)
    {

    }

    Connector::~Connector() {

    }

    void Connector::set_new_connection_cb(const Connector::NewConnCallback &cb) {
        _new_conn_cb=cb;
    }

    void Connector::set_new_connection_cb(Connector::NewConnCallback &&cb) {
        _new_conn_cb=std::move(cb);
    }

    void Connector::start() {
        Status t=Disconnected;
        if(_status.compare_exchange_strong(t,Connecting)){
            _loop->run_in_loop(std::bind(&Connector::connect,this));
        }
    }


    void Connector::stop_in_loop() {

    }

    void Connector::cancel() {

        if(_status==Disconnected)
            return ;

        _status==Disconnected;

        _loop->run_in_loop(std::bind(&Connector::stop_in_loop,this));
    }

    void Connector::connect() {

        int fd=Socket::create_nonblocking_socket(_addr.get_family());
        int rt=Socket::connect(fd,_addr);

        int serrno=(rt==0)?0:errno;

        switch (serrno)
        {
            case 0:
            case EINPROGRESS:
            case EINTR:
            case EISCONN:
                connecting(fd);
                break;

            case EAGAIN:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case ECONNREFUSED:
            case ENETUNREACH:
                retry(fd);
                break;

            case EACCES:
            case EPERM:
            case EAFNOSUPPORT:
            case EALREADY:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:
                LOG_ERROR << "connect error in Connector::start_in_loop " << serrno;
                Socket::close(fd);
                break;

            default:
                LOG_ERROR << "Unexpected error in Connector::start_in_loop " << serrno;
                Socket::close(fd);
                break;
        }
    }


    void Connector::handle_write() {
        LOG_TRACE << "Connector::handleWrite " << _status;

        if (_status == Connecting)
        {
            int sockfd = _event.get_fd();
            int err = Socket::get_socket_error(sockfd);
            if (err)
            {
                LOG_WARN << "Connector::handleWrite - SO_ERROR = "
                         << err ;
                retry(sockfd);
            }
            else if (Socket::is_self_connect(sockfd))
            {
                LOG_WARN << "Connector::handleWrite - Self connect";
                retry(sockfd);
            }
            else {
                Status t = Connecting;
                if (_status.compare_exchange_strong(t, Connected)) {
                    if (_new_conn_cb)
                        _new_conn_cb(sockfd, InetAddress(Socket::get_peer_addr(sockfd)));
                }
                else {
                    Socket::close(sockfd);
                }
            }
        }
        else
        {
            // what happened?
            assert(_status == Disconnected);
        }
    }

    void Connector::handle_error() {

    }

    void Connector::restart() {

    }

    void Connector::connecting(int fd) {
        _event.set_fd(fd);
        _event.set_write_cb(std::bind(&Connector::handle_write,this));
        _event.set_error_cb(std::bind(&Connector::handle_error,this));

        _event.enable_write();
    }

    void Connector::retry(int fd) {

    }


}