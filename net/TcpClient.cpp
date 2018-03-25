//
// Created by lg on 18-3-13.
//

#include "TcpClient.h"
#include"TcpConnection.h"

namespace net{
    using std::placeholders::_1;
    using std::placeholders::_2;

    TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &nameArg)
    :_loop(loop)
     ,_connector(loop,serverAddr)
    ,_peer_addr(serverAddr)
    ,_name(nameArg)
    ,_connection(nullptr)
    ,_retry(false)
    ,_status(Disconnected)
    {
        _connector.set_new_connection_cb(std::bind(&TcpClient::new_connection,this,_1,_2));
    }

    TcpClient::~TcpClient() {

    }

    void TcpClient::connect() {
        Status t=Disconnected;
        if(_status.compare_exchange_strong(t,Connecting)){
            _connector.start();
        }
    }

    void TcpClient::disconnect() {
        Status t=Connecting;
        if(_status.compare_exchange_strong(t,Disconnected)){
            _connection->close();
        }
    }

    void TcpClient::stop() {
        Status t=Connected;
        if(_status.compare_exchange_strong(t,Disconnecting)){
            _connector.cancel();
        }
    }

    void TcpClient::retry() {

    }

    void TcpClient::new_connection(int fd, InetAddress addr) {

    }
}