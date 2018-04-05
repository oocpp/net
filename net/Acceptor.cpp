//
// Created by lg on 18-3-19.
//

#include <cstring>
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Log.h"

namespace net
{
    Accepter::Accepter(EventLoop *loop, const InetAddress &addr)noexcept
            : _loop(loop)
              , _fd(Socket::create_nonblocking_socket(addr.get_family()))
              , _addr(addr)
              , _event(loop, _fd, true, false)
    {
        Socket::bind(_fd, _addr);
    }

    Accepter::~Accepter()noexcept
    {
        assert(!_event.is_add_to_loop());

        Socket::close(_fd);
    }

    void Accepter::listen(int backlog)
    {
        Socket::listen(_fd, backlog);

        _event.set_read_cb(std::bind(&Accepter::handle_accept, this));

        _loop->run_in_loop(std::bind(&Event::attach_to_loop, &_event));
    }

    void Accepter::stop()
    {
        assert(_loop->in_loop_thread());

        _event.detach_from_loop();
    }

    void Accepter::handle_accept()
    {
        assert(_loop->in_loop_thread());

        InetAddress addr;
        int connfd = Socket::accept(_fd, addr);

        //LOG_INFO<<"accept :fd = "<<connfd;

        if (connfd < 0) {
            return;
        }

        _new_connection_cb(connfd, addr);
    }

    void Accepter::set_new_connection_cb(const Accepter::NewConnCallback &cb)
    {
        _new_connection_cb = cb;
    }

    void Accepter::set_new_connection_cb(Accepter::NewConnCallback &&cb)noexcept
    {
        _new_connection_cb = std::move(cb);
    }
}