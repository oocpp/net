//
// Created by lg on 17-4-19.
//

#include "TcpConnection.h"
#include "Buffer.h"
#include"Log.h"
#include "Socket.h"

namespace net{

    TcpConnection::TcpConnection(uint64_t id,EventLoop*loop,int sockfd,const InetAddress&local_addr,const InetAddress&peer_add)
    :_sockfd(sockfd)
     ,_id(id)
     ,_loop(loop)
     ,_event(loop,sockfd)
     ,_conn_status(Disconnected)
    ,_local_addr(local_addr)
    ,_peer_addr(peer_add){
        _event.set_read_cb(std::bind(&TcpConnection::handle_read, this));
        _event.set_write_cb(std::bind(&TcpConnection::handle_write, this));
    }

    TcpConnection::~TcpConnection() {
        LOG_TRACE;
        Socket::close(_sockfd);
    }

    void TcpConnection::close() {
        LOG_TRACE << "fd=" <<_sockfd ;
        _conn_status = Disconnecting;

        _loop->queue_in_loop(std::bind(&TcpConnection::handle_close,shared_from_this()));
    }

    void TcpConnection::attach_to_loop() {
        _conn_status = Connected;
        _event.enable_read();

        if (_connecting_cb) {
            _connecting_cb(shared_from_this());
        }
    }

    void TcpConnection::handle_write() {

    }

    void TcpConnection::handle_read() {

        auto r = _in_buff.read_from_fd(_sockfd);

        if (r.first > 0) {
            _message_cb(shared_from_this(), &_in_buff);
        }
        else if (r.first == 0) {
            handle_close();
        }
        else {
            errno = r.second;
            LOG_ERROR << "TcpConnection::handleRead";
            handle_error();
        }
    }

    void TcpConnection::handle_close() {
        _conn_status = Disconnecting;

        _event.disable_all();

        TCPConnPtr conn(shared_from_this());

        if (_connecting_cb) {
            _connecting_cb(conn);
        }

        if (_close_cb) {
            _close_cb(conn);
        }
        LOG_TRACE << " fd=" << _sockfd ;

        _conn_status = Disconnected;
    }

    void TcpConnection::handle_error() {
        int err = Socket::get_socket_error(_event.get_fd());
        LOG_ERROR << "TcpConnection::handleError - SO_ERROR = " << err;

        handle_close();
    }
}