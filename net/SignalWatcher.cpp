#include "SignalWatcher.h"
#include "Log.h"
#include <csignal>
#include <cassert>
#include"EventLoop.h"

namespace net {

    std::map<int, SignalEventCallback> SignalHandler::_signal{};
    EventLoop *SignalHandler::_loop = nullptr;

    SignalHandler sig_handler;

    void SignalHandler::set_loop(EventLoop *loop) {
        _loop = loop;
    }

    void SignalHandler::signal(int s, const SignalEventCallback &cb) {
        if (::signal(s, handle_signal) != SIG_ERR)
            _signal.insert(std::make_pair(s, cb));
        else {
            LOG_ERROR << "set signal " << s << " failed";
        }
    }

    void SignalHandler::signal(int s, SignalEventCallback &&cb) {
        if (::signal(s, &handle_signal) == SIG_ERR) {
            LOG_ERROR << "set signal " << s << " failed,errno " << errno;
        }
        else {
            _signal.insert(std::make_pair(s, std::move(cb)));
        }
    }

    void SignalHandler::ignore(int s) {
        ::signal(s, SIG_IGN);
    }

    void SignalHandler::clear(int s) {
        assert(_signal.find(s) != _signal.end());

        ::signal(s, SIG_DFL);
        _signal.erase(s);
    }

    void SignalHandler::handle_signal(int s) {
        assert(_signal.find(s) != _signal.end());
        assert(_loop != nullptr);

        _loop->run_in_loop([s] { _signal[s](s); });
    }
}
