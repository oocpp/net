#pragma once

#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Event.h"

namespace net
{
    class TcpConnection;

    class EventLoop;

    namespace impl
    {
        class Accepter
        {
        public:
            using NewConnCallback = std::function<void(int, const InetAddress &)>;

            Accepter(EventLoop *loop, const InetAddress &listen_addr, int backlog = SOMAXCONN) noexcept;

            ~Accepter()noexcept;

            Accepter(Accepter &&acc)noexcept;

            Accepter(const Accepter &) = delete;

            Accepter &operator=(const Accepter &)= delete;

            void set_new_connection_cb(const NewConnCallback &cb);

            void set_new_connection_cb(NewConnCallback &&cb)noexcept;

            void listen();

            void stop();

        private:
            void handle_accept();

        private:
            EventLoop *_loop;
            int _fd;
            InetAddress _listen_addr;
            int _backlog;
            
            Event _event;
            NewConnCallback _new_connection_cb;
        };
    }
}
