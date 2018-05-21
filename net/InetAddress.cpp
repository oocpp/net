#include <arpa/inet.h>
#include <cstring>
#include <cassert>
#include <cstddef>
#include "Log.h"
#include "InetAddress.h"

namespace net {
    InetAddress::InetAddress() noexcept
            :_addr6{}
    {
        static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6), "InetAddress is same size as sockaddr_in6");
        static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
        static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
        static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
        static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");
        static_assert(offsetof(InetAddress, _addr6) == 0, "addr6_ offset 0");
        static_assert(offsetof(InetAddress, _addr) == 0, "addr_ offset 0");
    }

    InetAddress::InetAddress(const std::string &ip, uint16_t port,  bool ipv4)
        :_addr6{}
    {
        _addr.sin_port = htons(port);

        if (ipv4) {
            _addr.sin_family = AF_INET;

            if (::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) <= 0) {
                LOG_ERROR << "inet_pton 失败";
            }
        }
        else {
            _addr.sin_family = AF_INET6;
            if (::inet_pton(AF_INET6, ip.c_str(), &_addr6.sin6_addr) <= 0) {
                LOG_ERROR << "inet_pton 失败";
            }
        }
    }

    InetAddress::InetAddress(const struct sockaddr_in &addr)
            : _addr(addr) {
        assert(addr.sin_family == AF_INET);
    }


    InetAddress::InetAddress(const sockaddr_in6 &addr)
            : _addr6(addr) {
    }


    InetAddress::InetAddress(uint16_t port, bool addr_any, bool ipv4)
            :_addr6{}
    {
        _addr.sin_port = htons(port);

        if (ipv4) {
            _addr.sin_family = AF_INET;
            _addr.sin_addr.s_addr = htonl(addr_any? INADDR_ANY:INADDR_LOOPBACK);
        }
        else {
            _addr6.sin6_family = AF_INET6;
            _addr6.sin6_addr = addr_any?in6addr_any:in6addr_loopback;
        }
    }

    std::string InetAddress::to_ip() const {
        char buff[64] = {0};

        auto size = static_cast<socklen_t>(sizeof(buff));

        if (_addr.sin_family == AF_INET) {
            static_assert(sizeof(buff) >= INET_ADDRSTRLEN, "");
            ::inet_ntop(AF_INET, &_addr.sin_addr, buff, size);
        } else {
            assert(family() == AF_INET6);
            static_assert(sizeof(buff) >= INET6_ADDRSTRLEN, "");
            ::inet_ntop(AF_INET6, &_addr6.sin6_addr, buff, size);
        }
        return std::string(buff);
    }

    std::string InetAddress::to_ip_port() const {
        return to_ip().append(":").append(std::to_string(to_port()));
    }

    in_port_t InetAddress::to_port() const {
        return ntohs(_addr.sin_port);
    }

    const sockaddr *InetAddress::to_sockaddr() const {
        return reinterpret_cast<const sockaddr *>(&_addr);
    }

    sockaddr *InetAddress::to_sockaddr() {
        return reinterpret_cast<sockaddr *>(&_addr);
    }

    sa_family_t InetAddress::family() const {
        return _addr.sin_family;
    }

    in_port_t InetAddress::port() const {
        return _addr.sin_port;
    }

    uint32_t InetAddress::ip() const {
        assert(family() == AF_INET);
        return _addr.sin_addr.s_addr;
    }

    bool InetAddress::operator==(const InetAddress &another) const noexcept {
        assert(family() == AF_INET || family() == AF_INET6);

        if (port() == another.port() && family() == another.family()) {
            if (_addr.sin_family == AF_INET) {
                return ip() == another.ip();
            }
            else {
                return memcmp(&_addr6.sin6_addr, &another._addr6.sin6_addr, size6()) == 0;
            }
        }
        return false;
    }
}