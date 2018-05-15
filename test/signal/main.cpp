//
// Created by lg on 18-5-15.
//

#include <iostream>
#include <Log.h>
#include <csignal>

using namespace std;
#include"EventLoop.h"
#include"SignalWatcher.h"


using namespace net;


int main() {

    EventLoop loop;
    SignalHandler::set_loop(&loop);

    sig_handler.signal(SIGCHLD,[](int s){
        LOG_INFO<<"signal "<<s <<" catched";
    });

    loop.run_after(3s,[]{raise(SIGCHLD);});
    loop.run_after(5s,[]{raise(SIGCHLD);});
    loop.run_after(7s,[]{
        sig_handler.clear(SIGCHLD);
        raise(SIGCHLD);
        LOG_INFO<<"signal cancel";
    });

    loop.run();

    LOG_INFO<<"exit";
}