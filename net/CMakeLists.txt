cmake_minimum_required(VERSION 3.9)
project(net)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)

set(SOURCE_FILES main.cpp Log.h Socket.h Socket.cpp InetAddress.h InetAddress.cpp Buffer.h Buffer.cpp
        TcpConnection.cpp TcpConnection.h Acceptor.cpp Acceptor.h Connector.cpp Connector.h Epoll.cpp EventLoopThread.h
        EventLoopThread.cpp Epoll.h CallBack.h EventLoop.h EventLoop.cpp TcpServer.cpp TcpServer.h Client.cpp Client.h EventLoopThreadPool.cpp EventLoopThreadPool.h Event.cpp Event.h Any.h)

add_executable(net ${SOURCE_FILES})