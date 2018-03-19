#include <iostream>

using namespace std;

#include"Server.h"
using namespace net;
#include<thread>
#include"Socket.h"
using namespace net::Socket;
#include<numeric>
#include "Any.h"
int main() {

Any a(12);
    EventLoop loop;

    Server s(&loop,InetAddress("127.0.0.1",8888),"AAAA",3);
    s.run();

    thread A([]{
        this_thread::sleep_for(3s);
        int fd=create_nonblocking_socket();
        InetAddress addr("127.0.0.1",8888);
        connect(fd,addr);

        this_thread::sleep_for(10s);
        Socket::close(fd);
    });

    loop.run();

    cout<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}