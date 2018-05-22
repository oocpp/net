#include "SignalWatcher.h"
#include "Log.h"
#include <csignal>
#include <cassert>
#include"EventLoop.h"

namespace net {

    std::map<int, SignalEventCallback> SignalHandler::_signal{};
    EventLoop *SignalHandler::_loop = nullptr;

    SignalHandler sig_handler;

    void SignalHandler::set_loop(EventLoop *loop)
    {
        _loop = loop;
    }

    void SignalHandler::signal(int s, const SignalEventCallback &cb)
    {
        assert(_loop!= nullptr);
        _loop->run_in_loop([this,s,&cb]{signal_in_loop(s,cb);});
    }

    void SignalHandler::ignore(int s)
    {
        assert(_loop!= nullptr);
        _loop->run_in_loop([this,s]{ignore_in_loop(s);});
    }

    void SignalHandler::cancel(int s)
    {
        assert(_loop!= nullptr);
        _loop->run_in_loop([this,s]{cancel_in_loop(s);});
    }

    void SignalHandler::signal_in_loop(int s, const SignalEventCallback &cb)
    {
        if (::signal(s, handle_signal) != SIG_ERR)
            _signal.insert(std::make_pair(s, cb));
        else {
            LOG_ERROR << "set signal " << s << " failed";
        }
    }

    void SignalHandler::ignore_in_loop(int s)
    {
        ::signal(s, SIG_IGN);
        _signal.erase(s);
    }

    void SignalHandler::cancel_in_loop(int s)
    {
        assert(_signal.find(s) != _signal.end());

        ::signal(s, SIG_DFL);
        _signal.erase(s);
    }

    void SignalHandler::handle_signal(int s)
    {
        assert(_loop != nullptr);
        _loop->run_in_loop([s] {handle_signal_in_loop(s); });
    }


    void SignalHandler::handle_signal_in_loop(int s)
    {
        assert(_signal.find(s) != _signal.end());

        _signal[s](s);
    }
}
