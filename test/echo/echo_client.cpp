#include <iostream>
#include <unistd.h>
#include "TcpClient.h"
#include "NetFwd.h"
#include "Socket.h"
#include <string>

using namespace std;
using namespace net;


int main() {
    EventLoop loop;

    TcpClient client(&loop,InetAddress("127.0.0.1", 55555),"ChatClient");

    client.set_message_cb([](const TCPConnPtr& conn, Buffer* msg) {
        LOG_INFO<<"receive:"<<msg->get_read_ptr();
    });

    Socket::setNonBlockAndCloseOnExec(STDIN_FILENO);

    impl::Event e{&loop,STDIN_FILENO};

    e.set_read_cb([&client]{
        string s;
        getline(cin,s);
        client.get_conn()->send(s);
    });

    client.set_connection_cb([&e](const TCPConnPtr& conn) {
        if(conn->is_connected())
            e.enable_read();
        else
            e.disable_all();
    });

    client.set_message_cb([&e](const TCPConnPtr& conn,Buffer*msg) {
        msg->append("\0",1);
        LOG_INFO<<"receive:"<<msg->get_read_ptr();
        msg->clear();
    });

    client.connect();
    loop.run();
}