#pragma once

#include <string>
#include <map>
#include <vector>
#include "CallBack.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "Acceptor.h"

namespace net
{
    class EventLoop;

    class TcpConnection;

    class TcpServer
    {
    public:
        /// threadSize 为0，意为着server为单线程模式，所有操作均在loop线程
        /// threadSize 为1，意味着IO操作在新线程，listen在loop线程
        /// threadSize 为N，意味着会有一个线程数为N的线程池，IO操作均在线程池内处理
        TcpServer(EventLoop *loop, const InetAddress &addr, const std::string &name = "Server", size_t threadSize = 0);

        TcpServer(EventLoop *loop, const InetAddress &addr,int backlog = SOMAXCONN,
                const std::string &name = "Server", size_t threadSize = 0);

        ~TcpServer()noexcept;

        TcpServer(const TcpServer &) = delete;
        TcpServer &operator=(const TcpServer &)= delete;
        TcpServer(TcpServer &&) = delete;
        TcpServer &operator=(TcpServer &&)= delete;

        /// 添加新的监听，可同时监听多个ip:port
        void add_acceptor(const InetAddress &addr,int backlog = SOMAXCONN);

        /// 仅当构造函数未指定线程数量，
        /// 或者给定的参数值为0时可以调用这个函数
        void set_thread_size(size_t s);

        size_t size() const noexcept;

        /// 线程安全
        /// 开始server。开始监听，处理accept
        void run();

        /// 线程安全
        /// 停止server。主动强制断开所有连接
        void stop();

        /// 所有回调函数应在server状态为Stop时，设置

        void set_connection_cb(const ConnectingCallback &cb);

        void set_message_cb(const MessageCallback &cb);

        void set_write_complete_cb(const WriteCompleteCallback &cb);

        void set_connection_cb(ConnectingCallback &&cb) noexcept;

        void set_message_cb(MessageCallback &&cb) noexcept;

        void set_write_complete_cb(WriteCompleteCallback &&cb)noexcept;

        const std::string& name() const;

        EventLoop* loop()noexcept;

    private:
        void handle_new_connection(int fd, const InetAddress &addr);

        void remove_connection(const TCPConnPtr &conn);

        void remove_connection_in_loop(const TCPConnPtr &conn);

        void stop_in_loop();

        EventLoop *next_loop();

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

        std::vector<impl::Accepter> _accepters;

        uint64_t _next_conn_id = 0;
        std::map<uint64_t, TCPConnPtr> _connections;

        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        WriteCompleteCallback _write_complete_cb;
    };

}

