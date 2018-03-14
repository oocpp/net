//
// Created by lg on 17-4-21.
//
#include<functional>
#include"EventLoopThread.h"

namespace  net
{
    EventLoopThread::~EventLoopThread()noexcept {
        if(_th.joinable()){
            _loop.stop();
            _th.join();
        }
    }

    EventLoopThread::EventLoopThread()noexcept{

    }

    void EventLoopThread::join(){
        _th.join();
    }

    void EventLoopThread::run() {
        _loop.init();
        _th=std::thread(&EventLoop::run,&_loop);
    }

    void EventLoopThread::stop() {
        _loop.stop();
    }

    EventLoop &EventLoopThread::getLoop() {
        return _loop;
    }
}