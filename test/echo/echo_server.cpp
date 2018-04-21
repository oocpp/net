#include"TcpServer.h"
#include"NetFwd.h"

using namespace net;

int main(int argc, char* argv[]) {
    InetAddress addr("127.0.0.1",55555);

    EventLoop loop;

    TcpServer server(&loop, addr, "TCPEchoServer", 3);

    server.set_message_cb([](const TCPConnPtr& conn, Buffer* msg) {
        conn->send(msg);
    });

    server.set_connection_cb([](const TCPConnPtr & conn) {
        if (conn->is_connected()) {
            LOG_INFO << "A new connection from " << conn->get_peer_addr().toIpPort();
        } else {
            LOG_INFO << "Lost the connection from " << conn->get_peer_addr().toIpPort();
        }
    });

    server.run();
    loop.run();
}