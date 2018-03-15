//
// Created by lg on 18-3-13.
//

#include "Server.h"
#include<thread>

net::Server::Server(EventLoop*loop,size_t threadSize):_pool(threadSize),_loop_ptr(loop) {

}

void net::Server::run() {
    _pool.run();
}
