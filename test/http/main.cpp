#include <iostream>
#include"EventLoop.h"
#include"TcpServer.h"
#include"Buffer.h"
#include"TcpConnection.h"
#include"Log.h"

using namespace std;
using namespace net;

void http_echo(const TCPConnPtr &conn, Buffer *buff) {
    string str = "HTTP/1.1 200 OK\r\n"
                 "Server: net server\r\n"
                 "Content-Length: 7000\r\n"
                 "Content-Type: text/html\r\n\r\n<html>net server echo</html>";
    str.resize(7114);

    conn->send(str);
    LOG_INFO << "send";
    //conn->close();
}

void new_conn(const TCPConnPtr &conn) {
    if (conn->is_connected()) {
        LOG_INFO << "new connection" << conn->fd();
    }
}

int main() {
   // Log::set_rank(Log::INFO);
    EventLoop loop;

    InetAddress addr("127.0.0.1", 55555);
    TcpServer ser(&loop, addr, "http server", 3);

    ser.set_message_cb(http_echo);
    ser.set_connection_cb(new_conn);

    ser.run();
    loop.run();
}