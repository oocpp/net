cmake_minimum_required(VERSION 3.9)
project(net)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

include(CheckFunctionExists)
check_function_exists(accept4 HAVE_ACCEPT4)

set(SOURCE_FILES
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
        Log.cpp
        )

SET(CMAKE_CXX_FLAGS_DEBUG  "$ENV{CXXFLAGS} -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE   "$ENV{CXXFLAGS} -Wall")

set(NET_LIBRARY 0)

if(NET_LIBRARY)
    add_library(net ${SOURCE_FILES})
else()
    add_executable(net main.cpp ${SOURCE_FILES})
endif()