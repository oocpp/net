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


        void close();

        void set_message_cb(const MessageCallback & cb){
            _read_cb=cb;
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

        void reset_write_cb(){
            _write_cb= nullptr;
        }

        void reset_read_cb(){
            _read_cb= nullptr;
        }

        void enable_write(){
        }

        void enable_read(){
        }


        uint64_t get_id()const{
            return _id;
        }

        void attach_to_loop();
    private:
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
        MessageCallback _read_cb;

        std::function<void (TCPConnPtr&)> _write_cb;

        std::function<void (TCPConnPtr&)> _error_cb;
        std::function<void (TCPConnPtr&)> _write_high_level_cb;
        std::function<void (TCPConnPtr&)> _write_complete_cb;
    };
}
