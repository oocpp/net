#include <iostream>
//#include "Log.h"

using namespace std;
#include"EventLoop.h"
#include"TcpServer.h"
using namespace net;

int main() {


        EventLoop loop;
        TcpServer s(&loop,InetAddress("127.0.0.1",8888),"AAAA",1);




        s.run();
        //LOG_INFO<<"------------------------"<<endl;
        loop.run();



    //LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}