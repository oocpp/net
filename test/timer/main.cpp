#include <iostream>
#include <Log.h>
#include<chrono>
using namespace std;
#include"EventLoop.h"
using namespace net;

int main() {
    Log::set_rank(Log::INFO);
    EventLoop loop;

    auto timer=loop.run_every(3s,[](){
        time_t t=time(nullptr);
        cout<<ctime(&t);
        std::cout << "Hello, World!" << std::endl;
    });

    loop.run_after(5s,[](){
        time_t t=time(nullptr);
        cout<<ctime(&t);
        std::cout << "AAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    });

    loop.run_at(chrono::system_clock::now()+7s,[timer,&loop](){
        time_t t=time(nullptr);
        cout<<ctime(&t);
        std::cout << "BBBBBBBBBBBBBB" << std::endl;
        loop.cancel(timer);
    });

    cout<<"-----------------------------------"<<endl;

    thread A([&loop](){
        this_thread::sleep_for(19s);
        loop.stop();
    });

    loop.run();

    A.join();
}