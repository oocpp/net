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
    loop.run();

    TcpServer s(loop.get_loop(),InetAddress("127.0.0.1",8888),"AAAA",1);
    s.run();
    //this_thread::sleep_for(2s);



    LOG_INFO<<"------------------------"<<endl;
    //this_thread::sleep_for(5s);
    s.stop();

    //this_thread::sleep_for(20s);
    loop.stop();

    loop.join();

    LOG_INFO<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
}