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
#include "Connector.h"
#include<chrono>


    void fun(EventLoop&loop,TcpServer&s){
        this_thread::sleep_for(10s);
        s.stop();
        loop.stop();
    }

    int main() {


        EventLoop loop;
        TcpServer s(&loop,InetAddress("127.0.0.1",8888),"AAAA",1);

        thread B(fun,ref(loop),ref(s));

        thread A([&loop]{
            //this_thread::sleep_for(3s);
            Connector c(&loop,InetAddress("127.0.0.1",8888));

            this_thread::sleep_for(3s);
            c.start();
            this_thread::sleep_for(20s);
        });


        s.run();
        LOG_INFO<<"------------------------"<<endl;
        loop.run();



        B.join();
A.join();
    //loop.join();


    LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}