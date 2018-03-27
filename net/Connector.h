//
// Created by lg on 17-4-19.
//

#pragma once
#include <functional>
#include <atomic>
#include "InetAddress.h"
#include "Event.h"
#include<chrono>

namespace net{

    class EventLoop;

    class Connector:public std::enable_shared_from_this<Connector> {
    public:
        using NewConnCallback = std::function<void(int, const InetAddress &)>;

        Connector(EventLoop*loop,const InetAddress&addr);
        ~Connector();

        void set_new_connection_cb(const NewConnCallback&cb);

        void set_new_connection_cb(NewConnCallback&&cb);
        void start();
        void cancel();

        void restart();
    private:
       void retry(int fd);
        void connect();
        void connecting(int fd);
        void stop_in_loop();
        void handle_write();
        void handle_error();

        enum Status {
            Disconnected = 0,
            Connecting = 1,
            Connected = 2,
           // Disconnecting = 3,
        };

        static constexpr int init_retry_delay_ms=1000;
        static constexpr int max_retry_delay_ms=30*1000;
    private:
       EventLoop* _loop;
        InetAddress _addr;
        NewConnCallback _new_conn_cb;
        std::atomic<Status> _status;
        Event _event;
        std::chrono::milliseconds _retry_delay_ms;

    };
}