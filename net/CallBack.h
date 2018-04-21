#pragma once

#include <memory>
#include <functional>

namespace net
{
    class TcpConnection;

    class Buffer;

    class InetAddress;

    using TCPConnPtr = std::shared_ptr<TcpConnection>;

    using MessageCallback = std::function<void(const TCPConnPtr &, Buffer *)>;

    using ConnectingCallback =std::function<void(const TCPConnPtr &)>;

    using CloseCallback = std::function<void(const TCPConnPtr &)>;

    using WriteCompleteCallback = std::function<void(const TCPConnPtr &)>;

    using HighWaterMarkCallback = std::function<void(const TCPConnPtr &, size_t)> ;

    using EventCallback=std::function<void()>;
}
