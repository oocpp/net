#pragma once

#include <atomic>
#include "CallBack.h"
#include "InetAddress.h"
#include "Any.h"

namespace net
{
    class EventLoop;

    namespace impl{
        class Connector;
    }

    class TcpClient
    {
    public:
        TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &nameArg = "Client");

        ~TcpClient()noexcept;
        TcpClient(const TcpClient &) = delete;
        TcpClient &operator=(const TcpClient &)= delete;
        TcpClient(TcpClient &&) = delete;
        TcpClient &operator=(TcpClient &&)= delete;

        /// 线程安全
        /// 主动连服务器，会不断尝试连接
        void connect();

        /// 线程安全
        /// 断开连接，只是执行了shtudown write。
        /// 需要等待连接断开，才能停止loop
        void disconnect();

        /// 线程安全
        /// 强制断开连接，可以立即停止loop
        void force_disconnect();

        /// 设置连接中断是否重新连接
        void set_retry(bool t = true) noexcept;

        EventLoop *loop() const noexcept;

        /// 线程安全
        /// 返回std::shared_ptr<TcpConnection>对象
        /// 客户端重连后对象会失效
        TCPConnPtr connection() const;

        const std::string &name();

        /// 所有回调函数应在client状态为Disconnected时，设置

        void set_connection_cb(const ConnectingCallback &cb);

        void set_message_cb(const MessageCallback &cb);

        void set_write_complete_cb(const WriteCompleteCallback &cb);

        void set_connection_cb(ConnectingCallback &&cb)noexcept;

        void set_message_cb(MessageCallback &&cb)noexcept;

        void set_connect_failed_cb(const ConnectionFailedCallback&cb);

        void set_connect_failed_cb(ConnectionFailedCallback&&cb) noexcept;

        void set_write_complete_cb(WriteCompleteCallback &&cb)noexcept;

        /// context 可用来保存自定义的数据
        void set_context(const Any &a);

        void set_context(Any &&a);

        Any &get_context();

    private:
        void disconnect_in_loop();

        void force_disconnect_in_loop();

        void on_new_connection(int fd, const InetAddress &addr);

        void on_remove_connection(const TCPConnPtr &conn);

        void handle_connect_failed(int fd, const InetAddress &addr);

        enum Status
        {
            Disconnected = 0,
            Connecting = 1,
            Connected = 2,
            //Disconnecting = 3,
        };

        static std::atomic<uint64_t> id;
        using ConnectorPtr = std::shared_ptr<impl::Connector>;
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
        ConnectionFailedCallback  _conn_failed_cb;
        Any _context;
    };
}
