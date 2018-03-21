#include <iostream>

using namespace std;

#include"TcpServer.h"
using namespace net;
#include<thread>
#include"Socket.h"
using namespace net::Socket;
#include<numeric>
#include "Any.h"
#include"Log.h"
int main() {

Any a(12);
    EventLoop loop;


    TcpServer s(&loop,InetAddress("127.0.0.1",8888),"AAAA",3);
    s.run();
    //this_thread::sleep_for(2s);
    thread A([]{
        this_thread::sleep_for(3s);
        int fd=create_nonblocking_socket();
        Socket::connect(fd,InetAddress{"127.0.0.1",8888});

        this_thread::sleep_for(5s);
        Socket::close(fd);
    });
    loop.run();

    LOG_INFO<<"------------------------"<<endl;


    this_thread::sleep_for(7s);
    s.stop();

    //this_thread::sleep_for(20s);
    loop.stop();

    //loop.join();
    A.join();

    LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}