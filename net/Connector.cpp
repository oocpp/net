#include "Connector.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Log.h"

namespace net
{
    Connector::Connector(EventLoop *loop, const InetAddress &addr)noexcept
            : _loop(loop)
              , _addr(addr)
              , _status(Disconnected)
              , _event(loop, -1)
              , _retry_delay_ms(init_retry_delay_ms + 0)
    {

    }

    Connector::~Connector() noexcept
    {
        assert(_status != Connecting);
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
        assert(_status == Disconnected);

        Status t = Disconnected;
        if (_status.compare_exchange_strong(t, Connecting)) {
           // _loop->run_in_loop(std::bind(&Connector::connect, this));
            _loop->run_in_loop([this]{connect();});
        }
    }

    void Connector::stop_in_loop()
    {
        assert(_loop->in_loop_thread());

        _event.disable_all();
        Socket::close(_event.get_fd());

        assert(!_event.is_add_to_loop());
    }

    void Connector::cancel()
    {
        assert(_status == Connecting);

        Status t = Connecting;
        if (_status.compare_exchange_strong(t, Disconnected)) {
            //_loop->run_in_loop(std::bind(&Connector::stop_in_loop, this));
            _loop->run_in_loop([this]{stop_in_loop();});
        }
    }

    void Connector::connect()
    {
        if (_status != Connecting)
            return;

        int fd = Socket::create_nonblocking_socket(_addr.get_family());
        int rt = Socket::connect(fd, _addr);

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
                Socket::close(fd);
                break;

            default:
                LOG_ERROR << "Unexpected error in Connector::start_in_loop " << serrno;
                Socket::close(fd);
                break;
        }

        LOG_TRACE << "connecting .........";
    }


    void Connector::handle_write()
    {
        LOG_TRACE << "state=" << _status;

        _event.disable_all();

        if (_status == Connecting) {
            int sockfd = _event.get_fd();

            int err = Socket::get_socket_error(sockfd);
            if (err) {
                LOG_WARN << " SO_ERROR = " << err;
                retry(sockfd);
            }
            else if (Socket::is_self_connect(sockfd)) {
                LOG_WARN << "Self connect";
                retry(sockfd);
            }
            else {
                Status t = Connecting;
                if (_status.compare_exchange_strong(t, Connected)) {
                    LOG_TRACE << "connect success";
                    if (_new_conn_cb) {
                        _retry_delay_ms=std::chrono::milliseconds{init_retry_delay_ms};
                        _new_conn_cb(sockfd, InetAddress(Socket::get_local_addr(sockfd)));
                    }
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

    void Connector::handle_error()
    {
        LOG_ERROR << "state=" << _status;
        if (_status == Connecting) {
            int sockfd = _event.get_fd();

            _event.disable_all();

            int err = Socket::get_socket_error(sockfd);
            LOG_TRACE << "SO_ERROR = " << err << " " << err;
            retry(sockfd);
        }
    }

    void Connector::restart()
    {
        assert(_status == Connected);

        Status t = Connected;
        if (_status.compare_exchange_strong(t, Connecting)) {
            //_loop->run_in_loop(std::bind(&Connector::connect, this));
            _loop->run_in_loop([this]{connect();});
        }
    }

    void Connector::connecting(int fd)
    {
        if (_status != Connecting)
            return;

        _event.set_fd(fd);
        //_event.set_write_cb(std::bind(&Connector::handle_write, this));
        //_event.set_error_cb(std::bind(&Connector::handle_error, this));

        _event.set_write_cb([this]{handle_write();});
        _event.set_error_cb([this]{handle_error();});


        _event.enable_write();
    }

    void Connector::retry(int sockfd)
    {
        Socket::close(sockfd);

        if (_status == Connecting) {

            LOG_INFO << "Retry connecting to " << _addr.toIpPort()
                     << " in " << _retry_delay_ms.count() << " milliseconds. ";

            _retry_delay_ms *= 2;
            if (_retry_delay_ms.count() > max_retry_delay_ms)
                _retry_delay_ms = std::chrono::milliseconds(max_retry_delay_ms + 0);

            //_loop->run_after(_retry_delay_ms, std::bind(&Connector::connect, shared_from_this()));

            auto temp=shared_from_this();
            _loop->run_after(_retry_delay_ms, [temp]{temp->connect();});

        }
        else {
            LOG_DEBUG << "do not connect";
        }
    }
}