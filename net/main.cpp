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
#include "TcpClient.h"
#include<chrono>


    void fun(EventLoop&loop,TcpServer&s){
        this_thread::sleep_for(15s);
        s.stop();
        loop.stop();
    }

    int main() {


        EventLoop loop;
        TcpServer s(&loop,InetAddress("127.0.0.1",8888),"AAAA",1);

        thread B(fun,ref(loop),ref(s));

        thread A([&loop]{
            //this_thread::sleep_for(3s);
            TcpClient c(&loop,InetAddress("127.0.0.1",8888),"asdasd");

            this_thread::sleep_for(3s);
            c.connect();
            this_thread::sleep_for(5s);
            c.disconnect();
        });


        s.run();
        LOG_INFO<<"------------------------"<<endl;
        loop.run();



        B.join();
A.join();
    //loop.join();


    LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}