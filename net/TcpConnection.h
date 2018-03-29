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

        void set_high_water_cb(const HighWaterMarkCallback &cb, size_t mark){
            _write_high_level_cb=cb;
            _high_level_mark=mark;
        }

        void set_connection_cb(const ConnectingCallback &cb){
            _connecting_cb=cb;
        }

        void set_write_complete_cb(const WriteCompleteCallback &cb){
            _write_complete_cb=cb;
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

        void send(const std::string& d);
        void send(Buffer* d);

        bool is_connected()const{return _status==Connected;}

        InetAddress get_local_addr()const{return _local_addr;}
        InetAddress get_peer_addr()const{return _peer_addr;}
    private:
        void handle_read();
        void handle_write();

        void handle_close();
        void handle_error();

        void send_in_loop(const std::string& message);

        enum Status {
            Disconnected = 0,
            //Connecting = 1,
            Connected = 2,
            Disconnecting = 3,
        };
    private:
        int _sockfd;
        uint64_t _id;
        EventLoop*_loop;
        Event _event;
        std::atomic<Status> _status;

        InetAddress _local_addr;
        InetAddress _peer_addr;
        Buffer _in_buff;
        Buffer _out_buff;

        Any _context;

        size_t _high_level_mark=64 * 1024 * 1024; // Default 128MB

        CloseCallback  _close_cb;
        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        HighWaterMarkCallback _write_high_level_cb;
        WriteCompleteCallback _write_complete_cb;
    };
}
