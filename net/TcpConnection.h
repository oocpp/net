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
#include"CallBack.h"
#include "Any.h"


namespace net{

    class EventLoop;

    class TcpConnection:public std::enable_shared_from_this<TcpConnection> {
    public:

        TcpConnection(uint64_t id,EventLoop*loop,int sockfd,const InetAddress&local_addr,const InetAddress&peer_addr);
        ~TcpConnection();

        void close();

        void set_message_cb(const MessageCallback & cb){
            _message_cb=cb;
        }

        void set_write_cb(const std::function<void(std::shared_ptr<TcpConnection> &)> &cb){
            _write_cb=cb;
        }

        void set_connection_cb(const ConnectingCallback &cb){
            _connecting_cb=cb;
        }

        void set_close_cb(const CloseCallback&cb){
            _close_cb=cb;
        }

        void enable_write(){

        }

        void enable_read(){
        }

        EventLoop*get_loop(){return _loop;}

        uint64_t get_id()const{
            return _id;
        }

        void attach_to_loop();

        void set_context(const Any&a){
            _context=a;
        }

        void set_context(Any&&a){
            _context=std::move(a);
        }

        Any&get_context(){
            return _context;
        }

    private:
        void handle_read();
        void handle_write();

        void handle_close();
        void handle_error();

        enum Status {
            Disconnected = 0,
            Connecting = 1,
            Connected = 2,
            Disconnecting = 3,
        };
    private:

        uint64_t _id;
        EventLoop*_loop;
        int _sockfd;
        Event _event;
        std::atomic<Status> _conn_status;

        Buffer _in_buff;
        Buffer _out_buff;
        InetAddress _local_addr;
        InetAddress _peer_addr;
        Any _context;


        CloseCallback  _close_cb;
        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;

        std::function<void (TCPConnPtr&)> _write_cb;

        std::function<void (TCPConnPtr&)> _error_cb;
        std::function<void (TCPConnPtr&)> _write_high_level_cb;
        std::function<void (TCPConnPtr&)> _write_complete_cb;
    };
}
