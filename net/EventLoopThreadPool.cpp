//
// Created by lg on 18-3-13.
//

#include "EventLoopThreadPool.h"


void net::EventLoopThreadPool::run() {
    for(auto &t:_threads)
        t.run();
}

void net::EventLoopThreadPool::stop() {
    for(auto &t:_threads)
        t.stop();
}

void net::EventLoopThreadPool::join() {
    for(auto &t:_threads)
        t.join();
}

net::EventLoopThreadPool::EventLoopThreadPool(size_t threadNum):_threads(threadNum),_loop_index(0){

}

size_t net::EventLoopThreadPool::nextLoopIndex() {
   size_t t=_loop_index++;
    _loop_index%=_threads.size();
    return t;
}

net::EventLoop &net::EventLoopThreadPool::getNextLoop() {
    return _threads[nextLoopIndex()].getLoop();
}
