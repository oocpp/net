//
// Created by lg on 17-4-21.
//

#include"EventLoopThread.h"

namespace  net
{
    EventLoopThread::~EventLoopThread() {
        if(_th.joinable()){
            _loop.stop();
            _th.join();
        }
    }

    EventLoopThread::EventLoopThread() {

    }

    void EventLoopThread::join(){
        _th.join();
    }

    void EventLoopThread::run() {
        _loop.run();
    }

    void EventLoopThread::stop() {
        _loop.stop();
    }
}