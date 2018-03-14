#include <iostream>

using namespace std;

#include"Server.h"
using namespace net;
#include<thread>

int main() {
    Server s;
    s.run();

    this_thread::sleep_for(5s);
}