//
// Created by lg on 17-4-18.
//
#pragma once

#include <string>
#include <netinet/in.h>


namespace net {
    class InetAddress final
    {
    public:

        InetAddress()= default;
        explicit InetAddress(const std::string &ip, in_port_t port = 0,sa_family_t family=AF_INET);


        const sockaddr* get_sockaddr() const { return reinterpret_cast<const sockaddr*>(&_addr); }
        sockaddr* get_sockaddr() { return reinterpret_cast<sockaddr*>(&_addr); }

        sa_family_t get_family() const { return _addr.sin_family; }
        in_port_t get_port() const { return _addr.sin_port; }

        static constexpr socklen_t get_sockaddr_size() {return static_cast<socklen_t>(sizeof(sockaddr_in));}

        std::string toIp() const;
        std::string toIpPort() const;
        in_port_t toPort() const;
        uint32_t ipNetEndian() const;


        // resolve hostname to IP address, not changing port or sin_family
        // return true on success.
        // thread safe
        static bool resolve(const std::string& hostname, InetAddress& result);
        // static std::vector<InetAddress> resolveAll(const char* hostname, uint16_t port = 0);

    private:
            sockaddr_in _addr;
    };
}