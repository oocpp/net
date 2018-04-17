#include "Connector.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Log.h"

namespace net
{
    Connector::Connector(EventLoop *loop, const InetAddress &addr)noexcept
            : _loop(loop)
              , _peer_addr(addr)
              , _status(Disconnected)
              , _event(loop, -1)
              , _retry_delay_ms(init_retry_delay_ms + 0)
    {
        LOG_TRACE;
        _event.set_write_cb([this] { handle_write(); });
        //_event.set_connect_error_cb([this]{handle_error(_event.get_fd());});
    }

    Connector::~Connector() noexcept
    {
        assert(_status != Connecting);
        LOG_TRACE;
    }

    void Connector::set_new_connection_cb(const Connector::NewConnCallback &cb)
    {
        _new_conn_cb = cb;
    }

    void Connector::set_new_connection_cb(Connector::NewConnCallback &&cb)noexcept
    {
        _new_conn_cb = std::move(cb);
    }

    void Connector::start()
    {
        LOG_TRACE << "state = " << _status;

        Status t = Disconnected;
        if (_status.compare_exchange_strong(t, Connecting)) {
            _loop->run_in_loop([this] { connect(); });
        }
    }

    void Connector::stop_in_loop()
    {
        assert(_loop->in_loop_thread());
        LOG_TRACE;

        _event.disable_all();
        Socket::close(_event.get_fd());

        assert(!_event.is_add_to_loop());
    }

    void Connector::cancel()
    {
        LOG_TRACE << "state = " << _status;

        if (_status == Disconnected)
            return;

        if (_status.exchange(Disconnected) == Connecting) {
            auto temp = shared_from_this();
            _loop->run_in_loop([temp] { temp->stop_in_loop(); });
        }
    }

    void Connector::connect()
    {
        LOG_TRACE << "state = " << _status;

        if (_status != Connecting)
            return;

        int fd = Socket::create_nonblocking_socket(_peer_addr.get_family());
        int rt = Socket::connect(fd, _peer_addr);

        int serrno = (rt == 0) ? 0 : errno;

        switch (serrno) {
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
                handle_error(fd);
                break;

            default:
                LOG_ERROR << "Unexpected error in Connector::start_in_loop " << serrno;
                handle_error(fd);
                break;
        }
    }

    void Connector::handle_write()
    {
        LOG_TRACE << "state = " << _status;

        _event.disable_all();

        if (_status == Connecting) {

            int sockfd = _event.get_fd();
            InetAddress local_addr{Socket::get_local_addr(sockfd)};

            int err = Socket::get_socket_error(sockfd);
            if (err) {
                LOG_WARN << " SO_ERROR = " << err;
                retry(sockfd);
            }
            else if (_peer_addr == local_addr) {
                LOG_WARN << "Self connect";
                retry(sockfd);
            }
            else {
                Status t = Connecting;

                if (_status.compare_exchange_strong(t, Connected)) {
                    assert(_new_conn_cb);
                    _new_conn_cb(sockfd, local_addr);

                    LOG_TRACE << "connect success";
                    _retry_delay_ms = std::chrono::milliseconds{init_retry_delay_ms + 0};
                }
                else {
                    Socket::close(sockfd);
                }
            }
        }
        else {
            assert(_status == Disconnected);
        }
    }

    void Connector::handle_error(int sockfd)
    {
        LOG_ERROR << "state = " << _status;

        if (_status == Connecting) {

            int err = Socket::get_socket_error(sockfd);
            LOG_ERROR << "SO_ERROR = " << err;

            if (_error_cb)
                _error_cb(sockfd, _peer_addr);

            _status = Disconnected;
        }
        Socket::close(sockfd);
    }

    void Connector::restart()
    {
        LOG_TRACE << "state = " << _status;

        Status t = Connected;
        if (_status.compare_exchange_strong(t, Connecting)) {
            _loop->run_in_loop([this] { connect(); });
        }
    }

    void Connector::connecting(int fd)
    {
        LOG_TRACE << "state = " << _status;

        if (_status != Connecting)
            return;

        _event.set_fd(fd);
        _event.enable_write();
    }

    void Connector::retry(int sockfd)
    {
        Socket::close(sockfd);

        if (_status == Connecting) {

            LOG_INFO << "Retry connecting to " << _peer_addr.toIpPort()
                     << " in " << _retry_delay_ms.count() << " milliseconds. ";

            _retry_delay_ms *= 2;
            if (_retry_delay_ms.count() > max_retry_delay_ms)
                _retry_delay_ms = std::chrono::milliseconds(max_retry_delay_ms + 0);

            auto temp = shared_from_this();
            _loop->run_after(_retry_delay_ms, [temp] { temp->connect(); });

        }
        else {
            LOG_DEBUG << "do not connect";
        }
    }
}