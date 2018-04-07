#include <iostream>
#include"EventLoop.h"
#include"TcpServer.h"
#include"Buffer.h"
#include"TcpConnection.h"
#include"Log.h"

using namespace std;
using namespace net;

void http_echo(const TCPConnPtr &conn, Buffer *buff)
{
    string str = "HTTP/1.1 200 OK\r\n"
                 "Server: net server\r\n"
                 "Content-Length: 30\r\n"
                 "Content-Type: text/html\r\n\r\n<html>net server echo</html>                                    ";

    conn->send(str);
    LOG_INFO << "send";
}

void new_conn(const TCPConnPtr &conn)
{
    if (conn->is_connected()) {
        LOG_INFO << "new connection";
    }
}


int main()
{
    Log::set_rank(0);
    EventLoop loop;

    InetAddress addr("127.0.0.1", 8888);

    TcpServer ser(&loop, addr, "http server", 3);

    InetAddress addr1("127.0.0.1", 9999);
    InetAddress addr2("127.0.0.1", 7777);

    ser.add_acceptor(addr1);
    ser.add_acceptor(addr2);

    ser.set_message_cb(http_echo);
    ser.set_connection_cb(new_conn);

    thread A([&loop, &ser]() {
        this_thread::sleep_for(20s);
        ser.stop();
        loop.stop();

        this_thread::sleep_for(20s);
        ser.stop();
        loop.stop();
    });

    ser.run();
    loop.run();
    LOG_INFO << "  退出";

    this_thread::sleep_for(5s);
    ser.run();
    loop.run();

    A.join();
}