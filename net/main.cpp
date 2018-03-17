#include <iostream>

using namespace std;

#include"Server.h"
using namespace net;
#include<thread>

int main() {
    EventLoop loop;

    Server s(&loop,InetAddress("127.0.0.1",8888),"AAAA",3);
    s.run();

    loop.run();

    cout<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}