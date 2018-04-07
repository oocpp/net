#pragma once

#include<memory>
#include<string>
#include<functional>
#include<map>
#include<vector>

#include"EventLoopThreadPool.h"
#include "InetAddress.h"
#include"Acceptor.h"
#include"CallBack.h"

namespace net
{
    class EventLoop;

    class TcpConnection;

    class TcpServer
    {
    public:
        TcpServer(EventLoop *loop, const InetAddress &addr, const std::string &name, size_t threadSize = 0);

        ~TcpServer()noexcept;

        TcpServer(const TcpServer &) = delete;
        TcpServer &operator==(const TcpServer &)= delete;

        void add_acceptor(const InetAddress &addr);

        void run();

        void stop();

        void set_connection_cb(const ConnectingCallback &cb);

        void set_message_cb(const MessageCallback &cb);

        void set_write_complete_cb(const WriteCompleteCallback &cb);

        void set_connection_cb(ConnectingCallback &&cb) noexcept;

        void set_message_cb(MessageCallback &&cb) noexcept;

        void set_write_complete_cb(WriteCompleteCallback &&cb)noexcept;

    private:
        void handle_new_connection(int fd, const InetAddress &addr);

        void remove_connection(const TCPConnPtr &conn);

        void remove_connection_in_loop(const TCPConnPtr &conn);

        void stop_in_loop();

        EventLoop *get_next_loop();

        enum Status
        {
            Running = 1,
            Stopping = 2,
            Stopped = 3,
        };
    private:
        EventLoop *_loop;
        EventLoopThreadPool _pool;
        InetAddress _addr;
        std::string _name;
        size_t _th_size;
        std::atomic<Status> _status;

        std::vector<Accepter> _accepters;

        uint64_t _next_conn_id = 0;
        std::map<uint64_t, TCPConnPtr> _connections;

        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        WriteCompleteCallback _write_complete_cb;
    };

}

