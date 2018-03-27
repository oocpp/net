cmake_minimum_required(VERSION 3.9)
project(net)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)

include(CheckFunctionExists)
check_function_exists(accept4 HAVE_ACCEPT4)

set(SOURCE_FILES
        main.cpp
        Socket.cpp
        InetAddress.cpp
        Buffer.cpp
        TcpConnection.cpp
        Acceptor.cpp
        Connector.cpp
        Epoll.cpp
        EventLoopThread.cpp
        EventLoop.cpp
        TcpServer.cpp
        TcpClient.cpp
        EventLoopThreadPool.cpp
        Event.cpp
        Timer.cpp
        TimerQueue.cpp
        )

add_executable(net ${SOURCE_FILES})