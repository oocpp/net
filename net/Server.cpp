//
// Created by lg on 18-3-13.
//

#include "Server.h"
#include<thread>

net::Server::Server():_pool(5),_loop_ptr(nullptr) {

}

void net::Server::run() {
    _pool.run();
}
