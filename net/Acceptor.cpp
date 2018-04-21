#include <cstring>
#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "Log.h"

namespace net
{
    namespace impl
    {
        Accepter::Accepter(EventLoop *loop, const InetAddress &listen_addr)noexcept
                : _loop(loop)
                  , _fd(-1)
                  , _listen_addr(listen_addr)
                  , _event(loop)
        {
            LOG_TRACE;
        }

        Accepter::~Accepter()noexcept
        {
            assert(!_event.is_add_to_loop());
            LOG_TRACE;
        }

        void Accepter::listen(int backlog)
        {
            LOG_TRACE;
            _fd = Socket::create_nonblocking_socket(_listen_addr.get_family());
            if (_fd < 0) {
                LOG_ERROR << "listen failed. fd = " << _fd;
                abort();
            }

            Socket::bind(_fd, _listen_addr);
            Socket::listen(_fd, backlog);

            _event.set_fd(_fd);
            _event.set_read_cb([this] { handle_accept(); });

            _loop->run_in_loop([this] { _event.enable_read(); });
        }

        void Accepter::stop()
        {
            assert(_loop->in_loop_thread());
            LOG_TRACE;

            _event.disable_all();
            Socket::close(_fd);

            assert(!_event.is_add_to_loop());
        }

        void Accepter::handle_accept()
        {
            assert(_loop->in_loop_thread());
            assert(_event.is_readable());

            InetAddress addr{};
            int connfd = -1;
            while ((connfd = Socket::accept(_fd, addr)) > 0) {
                assert(_new_connection_cb);
                _new_connection_cb(connfd, addr);
            }
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
                : _loop(acc._loop)
                  , _fd(acc._fd)
                  , _listen_addr(acc._listen_addr)
                  , _event(std::move(acc._event))
                  , _new_connection_cb(std::move(acc._new_connection_cb))
        {
            acc._loop = nullptr;
            acc._fd = -1;
        }
    }
}
