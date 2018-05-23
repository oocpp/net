#pragma once

#include <atomic>
#include "CallBack.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "Epoll.h"
#include "Any.h"
#include "Event.h"

namespace net
{
    class EventLoop;

    /// 不会建立连接，只是用来管理连接
    class TcpConnection : public std::enable_shared_from_this<TcpConnection>
    {
    public:

        TcpConnection(uint64_t id, EventLoop *loop, int sockfd, const InetAddress &local_addr
                      , const InetAddress &peer_addr);

        ~TcpConnection()noexcept;

        TcpConnection(const TcpConnection &) = delete;
        TcpConnection &operator=(const TcpConnection &)= delete;
        TcpConnection(TcpConnection &&) = delete;
        TcpConnection &operator=(TcpConnection &&)= delete;

        /// 线程安全
        /// 关闭连接，只是调用了shutdown write
        void close();

        /// 线程安全
        /// 强制主动断开连接
        void force_close(bool call_close_cb = true);

        void set_message_cb(const MessageCallback &cb);

        /// 设置发送缓冲区内容过多时调用的方法
        void set_high_water_cb(const HighWaterMarkCallback &cb, size_t mark);

        void set_write_complete_cb(const WriteCompleteCallback &cb);

        void set_message_cb(MessageCallback &&cb = nullptr)noexcept;

        void set_high_water_cb(HighWaterMarkCallback &&cb = nullptr, size_t mark = 0)noexcept;

        void set_write_complete_cb(WriteCompleteCallback &&cb = nullptr)noexcept;

        /// 被调用，在链接成功后，或者链接关闭后
        void set_connection_cb(const ConnectingCallback &cb);

        void set_connection_cb(ConnectingCallback &&cb = nullptr)noexcept;

        EventLoop *loop()noexcept;

        /// 对象的id
        uint64_t id() const noexcept;

        int fd() const noexcept;

        // context 可用来保存自定义的数据
        void set_context(const Any &a = Any{});

        void set_context(Any &&a = Any{})noexcept;

        Any &get_context();

        void send(const std::string &d);

        void send(const char *str, size_t len);

        void send(Buffer *d);

        bool is_connected() const noexcept;

        InetAddress get_local_addr() const noexcept;

        InetAddress get_peer_addr() const noexcept;

        void set_tcp_no_delay(bool on);

        void reserve_input_buffer(size_t len);
        
        void reserve_output_buffer(size_t len);

        friend class TcpClient;
        friend class TcpServer;
    protected: /// 用户不应该使用下面的函数

        void attach_to_loop();

        void set_close_cb(const CloseCallback &cb);

        void set_close_cb(CloseCallback &&cb)noexcept;

    private:
        void handle_read();

        void handle_write();

        void handle_close(bool call_close_cb=true);

        void shutdown_in_loop();

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
        impl::Event _event;
        std::atomic<Status> _status;

        InetAddress _local_addr;
        InetAddress _peer_addr;
        Buffer _in_buff;
        Buffer _out_buff;

        Any _context;

        size_t _high_level_mark = 64 * 1024 * 1024; // Default 64MB

        CloseCallback _close_cb;
        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        HighWaterMarkCallback _write_high_level_cb;
        WriteCompleteCallback _write_complete_cb;
    };
}
