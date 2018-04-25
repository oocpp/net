#include <arpa/inet.h>
#include <cstring>
#include <cassert>

#include "Log.h"
#include "InetAddress.h"

namespace net {
    static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6), "InetAddress is same size as sockaddr_in6");
    static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
    static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
    static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
    static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");

    InetAddress::InetAddress() noexcept
            :_addr6{}
    {
    }

    InetAddress::InetAddress(const std::string &ip, uint16_t port,  bool ipv6)
        :_addr6{}
    {
        static_assert(offsetof(InetAddress, _addr6) == 0, "addr6_ offset 0");
        static_assert(offsetof(InetAddress, _addr) == 0, "addr_ offset 0");

        _addr.sin_port = htons(port);

        if (!ipv6) {
            _addr.sin_family = AF_INET;

            if (::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) <= 0) {
                LOG_ERROR << "inet_pton 失败";
            }
        } else {
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


    InetAddress::InetAddress(uint16_t port, bool ipv6)
            :_addr6{}
    {
        std::memset(&_addr6, 0, size6());
        _addr.sin_port = htons(port);

        if (!ipv6) {
            _addr.sin_family = AF_INET;
            _addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        } else {
            _addr6.sin6_family = AF_INET6;
            _addr6.sin6_addr = in6addr_loopback;
        }
    }

    std::string InetAddress::toIp() const {
        char buff[64] = {0};

        auto size = static_cast<socklen_t>(sizeof(buff));

        if (_addr.sin_family == AF_INET) {
            static_assert(sizeof(buff) >= INET_ADDRSTRLEN, "");
            ::inet_ntop(AF_INET, &_addr.sin_addr, buff, size);
        } else {
            assert(get_family() == AF_INET6);
            static_assert(sizeof(buff) >= INET6_ADDRSTRLEN, "");
            ::inet_ntop(AF_INET6, &_addr6.sin6_addr, buff, size);
        }
        return std::string(buff);
    }

    std::string InetAddress::toIpPort() const {
        return toIp().append(":").append(std::to_string(toPort()));
    }

    in_port_t InetAddress::toPort() const {
        return ntohs(_addr.sin_port);
    }

    const sockaddr *InetAddress::get_sockaddr() const {
        return reinterpret_cast<const sockaddr *>(&_addr);
    }

    sockaddr *InetAddress::get_sockaddr() {
        return reinterpret_cast<sockaddr *>(&_addr);
    }

    sa_family_t InetAddress::get_family() const {
        return _addr.sin_family;
    }

    in_port_t InetAddress::get_port() const {
        return _addr.sin_port;
    }

    uint32_t InetAddress::get_ip() const {
        assert(get_family() == AF_INET);
        return _addr.sin_addr.s_addr;
    }

    bool InetAddress::operator==(const InetAddress &another) const noexcept {
        assert(get_family() == AF_INET || get_family() == AF_INET6);

        if (get_port() == another.get_port() && get_family() == another.get_family()) {
            if (_addr.sin_family == AF_INET) {
                return get_ip() == another.get_ip();
            } else {
                return memcmp(&_addr6.sin6_addr, &another._addr6.sin6_addr, size6()) == 0;
            }
        }
        return false;
    }
}