#pragma once

#include <functional>
#include <memory>
#include <atomic>
#include <chrono>
#include <mutex>
#include "InetAddress.h"
#include "Event.h"

namespace net
{
    class EventLoop;

    namespace impl
    {
        class Connector : public std::enable_shared_from_this<Connector>
        {
        public:
            using NewConnCallback = std::function<void(int, const InetAddress &)>;

            Connector(EventLoop *loop, const InetAddress &addr)noexcept;

            ~Connector()noexcept;

            Connector(const Connector &) = delete;

            Connector &operator=(const Connector &)= delete;

            void set_new_connection_cb(const NewConnCallback &cb);

            void set_new_connection_cb(NewConnCallback &&cb)noexcept;

            void set_connect_error_cb(const NewConnCallback &cb);

            void set_connect_error_cb(NewConnCallback &&cb)noexcept;

            void start();

            void cancel();

            void restart();

        private:
            void retry(int fd);

            void connect();

            void connecting(int fd);

            void stop_in_loop();

            void handle_write();

            void handle_error(int sockfd);

            enum Status
            {
                Disconnected = 0,
                Connecting = 1,
                Connected = 2,
                // Disconnecting = 3,
            };

            static constexpr int init_retry_delay_ms = 500;
            static constexpr int max_retry_delay_ms = 10 * 1000;
        private:
            EventLoop *_loop;
            InetAddress _peer_addr;
            NewConnCallback _new_conn_cb;
            NewConnCallback _error_cb;
            std::atomic<Status> _status;
            Event _event;
            std::chrono::milliseconds _retry_delay_ms;
        };
    }
}
