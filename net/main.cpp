#include <iostream>

using namespace std;

#include"Server.h"
using namespace net;
#include<thread>

int main() {
    EventLoop loop;

    Server s(&loop,5);
    s.run();

    //loop.run();
    this_thread::sleep_for(3s);
}