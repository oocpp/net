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

        void ignore(int s);

        void cancel(int s);

    private:
        void signal_in_loop(int s, const SignalEventCallback &cb);

        void ignore_in_loop(int s);

        void cancel_in_loop(int s);

        static void handle_signal(int s);

        static void handle_signal_in_loop(int s);

        static std::map<int, SignalEventCallback> _signal;
        static EventLoop *_loop;
    };

    extern SignalHandler sig_handler;
}

