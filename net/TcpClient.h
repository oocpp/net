//
// Created by lg on 18-3-13.
//
#pragma once

#include "EventLoop.h"
#include "Connector.h"
#include "Any.h"

namespace net
{
    class TcpClient
    {
    public:
        TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &nameArg);

        ~TcpClient()noexcept;
        TcpClient(const TcpClient &) = delete;
        TcpClient &operator==(const TcpClient &)= delete;

        void connect();

        void disconnect();

        void cancel_connect();

        void set_retry(bool t = true);

        EventLoop *get_loop();

        const std::string &get_name();

        void set_connection_cb(const ConnectingCallback &cb);

        void set_message_cb(const MessageCallback &cb);

        void set_write_complete_cb(const WriteCompleteCallback &cb);

        void set_connection_cb(ConnectingCallback &&cb)noexcept;

        void set_message_cb(MessageCallback &&cb)noexcept;

        void set_write_complete_cb(WriteCompleteCallback &&cb)noexcept;

        void set_context(const Any &a);

        void set_context(Any &&a);

        Any &get_context();

        bool is_connecting() const
        { return _status == Connecting; }

    private:
        void on_new_connection(int fd, const InetAddress &addr);

        void on_remove_connection(const TCPConnPtr &conn);

        enum Status
        {
            Disconnected = 0,
            Connecting = 1,
            Connected = 2,
            //Disconnecting = 3,
        };

        static std::atomic<uint64_t> id;
        using ConnectorPtr = std::shared_ptr<Connector>;
    private:
        EventLoop *_loop;
        ConnectorPtr _connector;
        InetAddress _peer_addr;
        std::string _name;
        TCPConnPtr _connection;
        std::atomic<bool> _retry;
        std::atomic<Status> _status;

        WriteCompleteCallback _write_complete_cb;
        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        Any _context;
    };
}



