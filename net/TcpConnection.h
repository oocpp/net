//
// Created by lg on 17-4-19.
//

#pragma once
#include <list>
#include <functional>
#include <memory>

#include "InetAddress.h"
#include "Buffer.h"
#include "Epoll.h"
#include"EventLoop.h"

namespace net{

    class EventLoop;

    class TcpConnection {
    public:

        TcpConnection(uint64_t id,EventLoop*loop,int sockfd,const InetAddress&local_addr,const InetAddress&peer_addr);


        void close();

        void set_message_cb(const std::function<void(std::shared_ptr<TcpConnection> &)> &cb){
            _read_cb=cb;
           // setReadable();
        }

        void set_write_cb(const std::function<void(std::shared_ptr<TcpConnection> &)> &cb){
            _write_cb=cb;
            //setWriteable();
        }

        void reset_write_cb(){
            _write_cb= nullptr;
            //setDisableWrite();
        }

        void reset_read_cb(){
            _read_cb= nullptr;
           // setDisableRead();
        }

        void enable_write(){
           // setWriteable();
        }
        void enable_read(){
            //setReadable();
        }

        enum  TcpState{CONNECTING,GOOD,FAILURE,BAD,CLOSING};

        void setTcpState(TcpState ts){
            _tcp_state=ts;
        }
        TcpState getTcpState()const{
            return _tcp_state;
        }

        uint64_t get_id()const{
            return _id;
        }

        void attach_to_loop();
        using TCPConnPtr=std::shared_ptr<TcpConnection>;
    private:

        uint64_t _id;
        EventLoop*_loop;
        int _sockfd;
        //uint32_t _eventType;
        //uint32_t _activeEventType;

        Buffer _in_buff;
        Buffer _out_buff;
        InetAddress _local_addr;
        InetAddress _peer_addr;

        TcpState _tcp_state;

        std::function<void (TCPConnPtr&)> _read_cb;

        std::function<void (TCPConnPtr&)> _write_cb;

        std::function<void (TCPConnPtr&)> _close_cb;
        std::function<void (TCPConnPtr&)> _error_cb;
        std::function<void (TCPConnPtr&)> _write_high_level_cb;
        std::function<void (TCPConnPtr&)> _write_complete_cb;
    };
}
