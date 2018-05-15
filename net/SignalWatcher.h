#pragma once

#include <map>
#include"CallBack.h"

namespace net {
    class EventLoop;

    class SignalHandler {
    public:
        static void set_loop(EventLoop *loop);

        SignalHandler()= default;

        void signal(int s, const SignalEventCallback &cb);

        void signal(int s, SignalEventCallback &&cb);

        void ignore(int s);

        void clear(int s);

    private:
        static void handle_signal(int s);

        static std::map<int, SignalEventCallback> _signal;
        static EventLoop *_loop;
    };

    extern SignalHandler sig_handler;
}

