//
// Created by lg on 17-4-19.
//

#include <sys/epoll.h>
#include "Epoll.h"
#include <unistd.h>
#include "Log.h"

namespace net
{
    Epoll::Epoll()
        :_epollfd(::epoll_create1(EPOLL_CLOEXEC))
        ,_old_size(init_event_vector_size){

        if(_epollfd<0){
            LOG_ERROR<<"epoll 失败";
        }
    }

    Epoll::~Epoll() {
        ::close(_epollfd);
    }

    void Epoll::eventAdd(int fd,epoll_event event) {

        if (::epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event) < 0)
        {
            LOG_ERROR << "epoll_ctl op =" << "EPOLL_CTL_DEL"<< " fd =" << fd;

        }
    }

    void Epoll::eventDel(int fd,epoll_event event) {

        if (::epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &event) < 0)
        {
            LOG_ERROR << "epoll_ctl op =" << "EPOLL_CTL_DEL"<< " fd =" << fd;
        }
    }

    void Epoll::eventUpdate(int fd,epoll_event event) {

        if (::epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event) < 0)
        {
            LOG_ERROR << "epoll_ctl op =" << "EPOLL_CTL_DEL"<< " fd =" << fd;

        }
    }

    const std::vector<epoll_event> & Epoll::wait(int timeoutMs) {
        _events.reserve(_old_size);

        int numEvents = ::epoll_wait(_epollfd,_events.data(), _old_size, timeoutMs);

        if (numEvents > 0)
        {
            _events.resize(numEvents);

            if (static_cast<size_t>(numEvents) == _old_size)
            {
                _old_size=_old_size*2;
            }
        }
        else if (numEvents == 0)
        {
            _events.resize(0);
            LOG_ERROR << "nothing happended";
        }
        else
        {
            _events.resize(0);
            if (errno != EINTR)
            {
                LOG_ERROR << "EPollPoller::poll()";
            }
        }

        return _events;
    }
}