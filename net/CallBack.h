//
// Created by lg on 17-4-20.
//

#pragma once

namespace net
{

    class TcpConnection;
    class InetAddress;

    using TCPConnPtr=std::shared_ptr<TcpConnection> ;
    using NewConnCallback=std::function<void(int,const InetAddress&)>;

    //using Nstd::function<void()>_new_connection_cb;
    using MessageCallback= std::function<void (TCPConnPtr&)>;


}
