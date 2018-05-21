#pragma once

#include <string>
#include <netinet/in.h>

namespace net
{
    class InetAddress final
    {
    public:
        InetAddress() noexcept ;

        InetAddress(const std::string &ip, uint16_t port, bool ipv4 = true);

        explicit InetAddress(const sockaddr_in &addr);

        explicit InetAddress(const sockaddr_in6 &addr);

        /// addr_any true:使用any地址
        /// false：使用回环地址
        explicit InetAddress(uint16_t port,bool addr_any = true,bool ipv4 = true);

        const sockaddr *to_sockaddr() const;

        sockaddr *to_sockaddr();

        sa_family_t family() const;

        in_port_t port() const;

        uint32_t ip() const;

        bool operator==(const InetAddress &addr) const noexcept ;

        static constexpr socklen_t size()
        {
            return static_cast<socklen_t>(sizeof(sockaddr_in));
        }

        static constexpr socklen_t size6()
        {
            return static_cast<socklen_t>(sizeof(sockaddr_in6));
        }

        std::string to_ip() const;

        std::string to_ip_port() const;

        in_port_t to_port() const;

    private:
        union
        {
            struct sockaddr_in _addr;
            struct sockaddr_in6 _addr6;
        };
    };
}