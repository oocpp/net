//
// Created by lg on 18-3-13.
//
#pragma once

#include "EventLoop.h"
#include "Connector.h"
#include "Any.h"

namespace net{
    class TcpClient {
    public:
        TcpClient(EventLoop* loop,
                  const InetAddress& serverAddr,
                  const std::string& nameArg);
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        void enable_retry(){_retry=true;}

        EventLoop*get_loop(){return _loop;}
        const std::string& get_name(){return _name;}

        void set_connection_cb(const ConnectingCallback& cb)
        { _connecting_cb = cb; }


        void set_message_cb(const MessageCallback& cb)
        { _message_cb = cb; }


        void set_write_complete_cb(const WriteCompleteCallback& cb)
        { _write_complete_cb = cb; }

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
        void on_new_connection(int fd, InetAddress addr);
        void on_remove_connection(const TCPConnPtr&conn);
        enum Status {
            Disconnected = 0,
            Connecting = 1,
             Connected = 2,
            Disconnecting = 3,
        };

        static std::atomic<uint64_t> id;
        using ConnectorPtr = std::shared_ptr<Connector>;
    private:
        EventLoop*_loop;
        std::string _name;
        InetAddress _peer_addr;
        ConnectorPtr _connector;
        std::atomic<bool>  _retry;
        std::atomic<Status>  _status;
        TCPConnPtr _connection;

        WriteCompleteCallback _write_complete_cb;
        ConnectingCallback _connecting_cb;
        MessageCallback _message_cb;
        Any _context;
    };

}



