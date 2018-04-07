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
        LOG_TRACE<<"fd = "<<_fd;
        if(_fd<0) {
            abort();
        }
        Socket::bind(_fd, _addr);
    }

    Accepter::~Accepter()noexcept
    {
        assert(!_event.is_add_to_loop());
        LOG_TRACE;
        Socket::close(_fd);
    }

    void Accepter::listen(int backlog)
    {
        LOG_TRACE;
        Socket::listen(_fd, backlog);

        //_event.set_read_cb(std::bind(&Accepter::handle_accept, this));
        //_loop->run_in_loop(std::bind(&Event::attach_to_loop, &_event));

        _event.set_read_cb([this]{handle_accept();});

        _loop->run_in_loop([this]{_event.attach_to_loop();});
    }

    void Accepter::stop()
    {
        assert(_loop->in_loop_thread());

        _event.detach_from_loop();

        assert(!_event.is_add_to_loop());
    }

    void Accepter::handle_accept()
    {
        assert(_loop->in_loop_thread());

        InetAddress addr{};
        int connfd = Socket::accept(_fd, addr);

        //LOG_INFO<<"accept :fd = "<<connfd;

        if (connfd < 0) {
            LOG_INFO<<"fd = "<<connfd;
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

    Accepter::Accepter(Accepter &&acc)noexcept
        :_loop(acc._loop)
        , _fd(acc._fd)
        ,_addr(acc._addr)
        ,_event(std::move(acc._event))
        ,_new_connection_cb(std::move(acc._new_connection_cb))
    {
        acc._loop=nullptr;
        acc._fd=-1;
    }
}