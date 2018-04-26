# net
# 个人毕设作品。。。。。。
# 目前初步完成。继续完善中。。。

net是一个基于Reactor 模式的现代化C++11网络库。

自带TCP协议的异步非阻塞式的服务器和客户端库。

实现上深度借鉴[muduo](https://github.com/chenshuo/muduo)和[evpp](https://github.com/Qihoo360/evpp)两个开源网络库。

性能上没太多测试，不过个人感觉性能还不错。。。

没啥第三方库依赖，只要支持c++11就行。。。

需要的linux版本没测试。。。有时间再说。。。

#### 使用时,需要确保TcpClient和TcpServer的生命周期至少在EvnetLoop::run()函数退出之后结束

# 特性

现代版的C++11接口

非阻塞异步接口都是C++11的functional形式的回调仿函数

非阻塞纯异步多线程TCP服务器/客户端

单核环境支持

线程安全

线程安全的退出，重启

客户端断线自动重连

服务端可同时监听多个ip/port

ipv6支持

# 例子
## Tcp Echo Server
```cpp
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
            LOG_INFO << "Lost the connection from " << conn->get_peer_addr().to_ip_port();
        }
    });

    server.run();
    loop.run();
}
```

## Tcp Echo Client
```cpp
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
        LOG_INFO<<"receive:"<<msg->read_ptr();
    });

    Socket::setNonBlockAndCloseOnExec(STDIN_FILENO);

    impl::Event e{&loop,STDIN_FILENO};

    e.set_read_cb([&client]{
        string s;
        getline(cin,s);
        client.connection()->send(s);
    });

    client.set_connection_cb([&e](const TCPConnPtr& conn) {
        if(conn->is_connected())
            e.enable_read();
        else
            e.disable_all();
    });

    client.set_message_cb([&e](const TCPConnPtr& conn,Buffer*msg) {
        msg->append("\0",1);
        LOG_INFO<<"receive:"<<msg->read_ptr();
        msg->clear();
    });

    client.connect();
    loop.run();
}
```
