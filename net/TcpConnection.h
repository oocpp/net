#pragma once

#include <list>
#include <functional>
#include <memory>

#include "InetAddress.h"
#include "Buffer.h"
#include "Epoll.h"
#include"EventLoop.h"
#include"CallBack.h"
#include "Any.h"

namespace net
{

    class EventLoop;

    class TcpConnection : public std::enable_shared_from_this<TcpConnection>
    {
    public:

        TcpConnection(uint64_t id, EventLoop *loop, int sockfd, const InetAddress &local_addr
                      , const InetAddress &peer_addr);

        ~TcpConnection()noexcept;

        TcpConnection(const TcpConnection &) = delete;
        TcpConnection &operator==(const TcpConnection &)= delete;

        void close(bool call_close_cb=true);

        void set_message_cb(const MessageCallback &cb);

        void set_high_water_cb(const HighWaterMarkCallback &cb, size_t mark);

        void set_connection_cb(const ConnectingCallback &cb);

        void set_write_complete_cb(const WriteCompleteCallback &cb);

        void set_close_cb(const CloseCallback &cb);

        void set_message_cb(MessageCallback &&cb)noexcept;

        void set_high_water_cb(HighWaterMarkCallback &&cb, size_t mark)noexcept;

        void set_connection_cb(ConnectingCallback &&cb)noexcept;

        void set_write_complete_cb(WriteCompleteCallback &&cb)noexcept;

        void set_close_cb(CloseCallback &&cb)noexcept;

        EventLoop *get_loop()noexcept;

        uint64_t get_id() const noexcept;

        int get_fd() const noexcept;

        void attach_to_loop();

        void set_context(const Any &a);

        void set_context(Any &&a)noexcept;

        Any &get_context();

        void send(const std::string &d);

        void send(const char *str, size_t len);

        void send(Buffer *d);

        bool is_connected() const noexcept;

        InetAddress get_local_addr() const noexcept;

        InetAddress get_peer_addr() const noexcept;

    private:
        void handle_read();

        void handle_write();

        void handle_close(bool call_close_cb=true);

        void handle_error();

        void send_in_loop(const char *str, size_t len);

        void send_string_in_loop(const std::string &str);

        enum Status
        {
            Disconnected = 0,
            //Connecting = 1,
            Connected = 2,
            Disconnecting = 3,
        };
    private:
        int _sockfd;
        uint64_t _id;
        EventLoop *_loop;
        Event _event;
        std::atomic<Status> _status;

        InetAddress _local_addr;
        InetAddress _peer_addr;
        Buffer _in_buff;
        Buffer _out_buff;

        Any _context;

        size_t _high_level_mark = 64 * 1024 * 1024; // Default 128MB

        CloseCallback _close_cb;
        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        HighWaterMarkCallback _write_high_level_cb;
        WriteCompleteCallback _write_complete_cb;
    };
}
