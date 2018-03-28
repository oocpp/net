//
// Created by lg on 17-4-19.
//

#include <cstring>
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
     ,_status(Disconnected)
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
        Status t = Connected;
        if(_status.compare_exchange_strong(t,Disconnecting)) {
            LOG_TRACE << "fd=" << _sockfd;
            _loop->queue_in_loop(std::bind(&TcpConnection::handle_close, shared_from_this()));
        }
    }

    void TcpConnection::attach_to_loop() {
        _status = Connected;
        _event.enable_read();

        if (_connecting_cb) {
            _connecting_cb(shared_from_this());
        }
    }

    void TcpConnection::handle_read() {
        assert(_loop->in_loop_thread());

        auto r = _in_buff.read_from_fd(_sockfd);

        if (r.first > 0) {
            _message_cb(shared_from_this(), &_in_buff);
        }
        else if (r.first == 0) {
            _status = Disconnecting;
            handle_close();
        }
        else {
            errno = r.second;
            LOG_ERROR << "TcpConnection::handleRead";
            handle_error();
        }
    }

    void TcpConnection::handle_close() {
        assert(_loop->in_loop_thread());

        if(_status==Disconnected)
            return;

        _status = Disconnecting;
        _event.disable_all();

        TCPConnPtr conn(shared_from_this());

        if (_connecting_cb) {
            _connecting_cb(conn);
        }

        if (_close_cb) {
            _close_cb(conn);
        }
        LOG_TRACE << " fd=" << _sockfd ;

        _status = Disconnected;
    }

    void TcpConnection::handle_error() {
        assert(_loop->in_loop_thread());

        int err = Socket::get_socket_error(_event.get_fd());
        LOG_ERROR << "TcpConnection::handleError - SO_ERROR = " << err;

        _status = Disconnecting;
        handle_close();
    }

    void TcpConnection::send(const std::string &d) {
        if(_status!=Connected)
            return ;

        if (_loop->in_loop_thread()) {
            send_in_loop(d);
            return;
        }
        else{
            _loop->run_in_loop(std::bind(&TcpConnection::send_in_loop, shared_from_this(), d));
        }
    }

    void TcpConnection::send_in_loop(const std::string &message) {
        if(_status==Disconnected)
            return ;

        ssize_t nwritten = 0;
        size_t remaining = message.size();
        bool write_error = false;

        if(!_event.is_write()&&_out_buff.length()==0){
            nwritten = ::send(_sockfd, message.data(), message.length(), MSG_NOSIGNAL);
            if (nwritten >= 0) {
                remaining = message.length() - nwritten;
                if (remaining == 0 && _write_complete_cb) {
                    _loop->queue_in_loop(std::bind(_write_complete_cb, shared_from_this()));
                }
            }
            else {
                int serrno = errno;
                nwritten = 0;
                if (serrno != EWOULDBLOCK) {
                    LOG_ERROR << "SendInLoop write failed errno=" << serrno << " " << strerror(serrno);
                    if (serrno == EPIPE || serrno == ECONNRESET) {
                        write_error = true;
                    }
                }
            }
        }

        if (write_error) {
            handle_error();
            return;
        }

        assert(!write_error);
        assert(remaining <= message.length());

        if (remaining > 0) {
            size_t old_len = _out_buff.length();
            if (old_len + remaining >= _high_level_mark && old_len < _high_level_mark
                && _write_high_level_cb) {
                _loop->queue_in_loop(std::bind(_write_high_level_cb, shared_from_this(), old_len + remaining));
            }

            _out_buff.append(static_cast<const char*>(message.data()) + nwritten, remaining);

            if (!_event.is_write()) {
                _event.enable_write();
            }
        }
    }

    void TcpConnection::handle_write() {
        assert(_loop->in_loop_thread());
        assert(!_event.is_add_to_loop() || _event.is_write());

        ssize_t n = ::send(_sockfd, _out_buff.get_read_ptr(), _out_buff.get_readable_size(), MSG_NOSIGNAL);
        if (n > 0) {
            _out_buff.has_read(n);

            if (_out_buff.get_readable_size() == 0) {
                _event.disable_write();

                if (_write_complete_cb) {
                    _loop->queue_in_loop(std::bind(_write_complete_cb, shared_from_this()));
                }
            }
        }
        else {
            int serrno = errno;

            if (serrno != EWOULDBLOCK) {
                LOG_WARN << "this=" << this << " TCPConn::HandleWrite errno=" << serrno << " " << strerror(serrno);
            } else {
                handle_error();
            }
        }
    }
}