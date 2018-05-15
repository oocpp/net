#pragma once

#include <vector>
#include <sys/epoll.h>

//EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
//EPOLLOUT：表示对应的文件描述符可以写；
//EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
//EPOLLERR：表示对应的文件描述符发生错误；
//EPOLLHUP：表示对应的文件描述符被挂断；
//EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
//EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

namespace net
{
    namespace impl
    {
        class Epoll
        {
        public:
            Epoll()noexcept;

            ~Epoll()noexcept;

            Epoll(const Epoll &) = delete;

            Epoll &operator=(const Epoll &)= delete;

            void add(int fd, epoll_event event);

            void remove(int fd);

            void update(int fd, epoll_event event);

            void wait(int timeout, std::vector<epoll_event> &events);

        private:
            int _epollfd;
            size_t _old_size;
            static constexpr size_t init_event_vector_size = 16;
        };
    }
}
