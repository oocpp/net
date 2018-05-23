#include <TcpClient.h>
#include <Log.h>
#include <EventLoop.h>
#include <EventLoopThreadPool.h>
#include <InetAddress.h>
#include<Buffer.h>
#include<TcpConnection.h>

#include <utility>

#include <stdio.h>
#include <unistd.h>
#include <TcpServer.h>

using namespace std;
using namespace net;
using namespace std::placeholders;


void onConnection(const TCPConnPtr& conn)
{
    if(conn->is_connected())
    conn->set_tcp_no_delay(true);
}

void onMessage(const TCPConnPtr& conn, Buffer* buf)
{
  conn->send(buf);
  buf->clear();
}

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    fprintf(stderr, "Usage: server <address> <port> <threads>\n");
  }
  else
  {
    Log::set_rank(Log::INFO);

    const char* ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    InetAddress listenAddr(ip, port);
    int threadCount = atoi(argv[3]);

    EventLoop loop;

    TcpServer server(&loop, listenAddr, "PingPong",threadCount);

    server.set_connection_cb(onConnection);
    server.set_message_cb(onMessage);

    server.run();

    loop.run();
  }
}

