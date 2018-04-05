//
// Created by lg on 17-4-18.
//
#pragma once

#include <string>
#include <netinet/in.h>


namespace net
{
    class InetAddress final
    {
    public:
        InetAddress() = default;

        explicit InetAddress(const std::string &ip, in_port_t port = 0, sa_family_t family = AF_INET);

        explicit InetAddress(const struct sockaddr_in &addr);

        const sockaddr *get_sockaddr() const;

        sockaddr *get_sockaddr();

        sa_family_t get_family() const;

        in_port_t get_port() const;

        static constexpr socklen_t get_sockaddr_size()
        {
            return static_cast<socklen_t>(sizeof(sockaddr_in));
        }

        std::string toIp() const;

        std::string toIpPort() const;

        in_port_t toPort() const;

        uint32_t ipNetEndian() const;

    private:
        sockaddr_in _addr;
    };
}