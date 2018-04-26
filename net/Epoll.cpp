#include <unistd.h>
#include "Epoll.h"
#include "Log.h"

namespace net
{
    namespace impl
    {
        Epoll::Epoll()noexcept
                : _epollfd(::epoll_create1(EPOLL_CLOEXEC))
                  , _old_size(init_event_vector_size)
        {
            if (_epollfd < 0) {
                LOG_ERROR << "epoll 失败";
                abort();
            }
        }

        Epoll::~Epoll()noexcept
        {
            ::close(_epollfd);
        }

        void Epoll::add(int fd, epoll_event event)
        {
            if (::epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event) < 0) {
                LOG_ERROR << "epoll_ctl op =" << "EPOLL_CTL_ADD" << " fd =" << fd;
            }
        }

        void Epoll::remove(int fd)
        {
            epoll_event event{};
            if (::epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event) < 0) {
                LOG_ERROR << "epoll_ctl op =" << "EPOLL_CTL_DEL" << " fd =" << fd;
            }
        }

        void Epoll::update(int fd, epoll_event event)
        {
            if (::epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event) < 0) {
                LOG_ERROR << "epoll_ctl op =" << "EPOLL_CTL_MOD" << " fd =" << fd;
            }
        }

        void Epoll::wait(int timeoutMs, std::vector<epoll_event> &events)
        {
            events.resize(_old_size);

            int numEvents = ::epoll_wait(_epollfd, events.data(), static_cast<int>(_old_size), timeoutMs);

            int savedErrno = errno;
            if (numEvents > 0) {
                events.resize(static_cast<size_t >(numEvents));

                if (static_cast<size_t>(numEvents) == _old_size) {
                    _old_size = _old_size * 2;
                }
            }
            else if (numEvents == 0) {
                events.resize(0);
                LOG_ERROR << "nothing happended";
            }
            else {
                events.resize(0);
                if (savedErrno != EINTR) {
                    errno = savedErrno;
                    LOG_ERROR << "EPollPoller::_poll()";
                }
            }
        }
    }
}
