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


    void fun(EventLoop&loop,TcpServer&s){
        s.run();
        LOG_INFO<<"------------------------"<<endl;
        loop.run();
    }

    int main() {
        EventLoop loop;
        TcpServer s(&loop,InetAddress("127.0.0.1",8888),"AAAA",3);

        thread B(fun,ref(loop),ref(s));

        thread A([]{
            this_thread::sleep_for(3s);
            int fd=create_nonblocking_socket();
            Socket::connect(fd,InetAddress{"127.0.0.1",8888});

            this_thread::sleep_for(3s);
            Socket::close(fd);
        });



        this_thread::sleep_for(10s);
        s.stop();
    loop.stop();
        B.join();

    //loop.join();
    A.join();

    LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}