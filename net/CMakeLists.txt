cmake_minimum_required(VERSION 3.9)
project(net)

set(CMAKE_CXX_STANDARD 14)

include(CheckFunctionExists)
check_function_exists(accept4 HAVE_ACCEPT4)

link_libraries(pthread)

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
SET(CMAKE_CXX_FLAGS_RELEASE  "$ENV{CXXFLAGS} -O3 -Wall")

set(NET_LIBRARY 1)

if(NET_LIBRARY)
    SET(lib_name "net")
    add_library(${lib_name} STATIC ${SOURCE_FILES})

    set_target_properties(${lib_name}
            PROPERTIES
            OUTPUT_NAME_DEBUG ${lib_name}d
            OUTPUT_NAME ${lib_name}
            )

    set(HEAD_FILES
            Socket.h
            InetAddress.h
            Buffer.h
            TcpConnection.h
            Acceptor.h
            Connector.h
            Epoll.h
            EventLoopThread.h
            EventLoop.h
            TcpServer.h
            TcpClient.h
            EventLoopThreadPool.h
            Event.h
            Timer.h
            TimerQueue.h
            Log.h
            Any.h
            CallBack.h
            )

    INSTALL(TARGETS ${lib_name} ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/../net_library/lib_static/)
    INSTALL(FILES ${HEAD_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/../net_library/include/)

else()
    add_executable(net main.cpp ${SOURCE_FILES})
endif()

