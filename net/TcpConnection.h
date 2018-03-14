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

namespace net{

    class TcpSession {
    public:

        explicit TcpSession(const InetAddress&peerAddr);

        TcpSession(int sockfd,const InetAddress&localAddr,const InetAddress&peerAddr);

        //EventBase&getEventBase();

        void setIter(std::list<std::unique_ptr<TcpSession>>::iterator iter){
            _iter=iter;
        }
        std::list<std::unique_ptr<TcpSession>>::iterator getIter() const{
            return _iter;
        }

        void close();

        void setReadCallback(const std::function<void (std::unique_ptr<TcpSession>&)>&cb){
            _read_cb=cb;
           // setReadable();
        }
        void setReadCallback(std::function<void (std::unique_ptr<TcpSession>&)>&&cb){
            _read_cb=std::move(cb);
            //setReadable();
        }

        void setWriteCallback(const std::function<void (std::unique_ptr<TcpSession>&)>&cb){
            _write_cb=cb;
            //setWriteable();
        }
        void setWriteCallback(std::function<void (std::unique_ptr<TcpSession>&)>&&cb){
            _write_cb=std::move(cb);
            //setWriteable();
        }

        void resetWriteCallback(){
            _write_cb= nullptr;
            //setDisableWrite();
        }

        void resetReadCallback(){
            _read_cb= nullptr;
           // setDisableRead();
        }

        void enableWrite(){
           // setWriteable();
        }
        void enableRead(){
            //setReadable();
        }

        enum  TcpState{CONNECTING,GOOD,FAILURE,BAD,CLOSING};

        void setTcpState(TcpState ts){
            _tcp_state=ts;
        }
        TcpState getTcpState()const{
            return _tcp_state;
        }

    private:

        int _sockfd;
        //uint32_t _eventType;
        //uint32_t _activeEventType;

        Buffer _buff;
        InetAddress _local_addr;
        InetAddress _peer_addr;

        TcpState _tcp_state;

       std::list<std::unique_ptr<TcpSession>>::iterator _iter;
        std::function<void (std::unique_ptr<TcpSession>&)> _read_cb;

        std::function<void (std::unique_ptr<TcpSession>&)> _write_cb;

        std::function<void (std::unique_ptr<TcpSession>&)> _close_cb;
        std::function<void (std::unique_ptr<TcpSession>&)> _error_cb;
        std::function<void (std::unique_ptr<TcpSession>&)> _write_high_level_cb;
        std::function<void (std::unique_ptr<TcpSession>&)> _write_complete_cb;
    };
}
