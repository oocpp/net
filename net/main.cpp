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
    EventLoopThread loop;


    TcpServer s(loop.get_loop(),InetAddress("127.0.0.1",8888),"AAAA",1);
    s.run();
    //this_thread::sleep_for(2s);
    loop.run();

    LOG_INFO<<"------------------------"<<endl;

    thread A([]{
        this_thread::sleep_for(2s);
        int fd=create_nonblocking_socket();
        Socket::connect(fd,InetAddress{"127.0.0.1",8888});

        this_thread::sleep_for(10s);
        Socket::close(fd);
    });

    this_thread::sleep_for(6s);
    s.stop();

    //this_thread::sleep_for(20s);
    loop.stop();

    loop.join();
    A.join();

    LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}