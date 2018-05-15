set(CMAKE_CXX_STANDARD 11)

include(CheckFunctionExists)
check_function_exists(accept4 HAVE_ACCEPT4)

link_libraries(${DEPENDENT_LIBRARIES})

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
        SignalWatcher.cpp
        )

set(target_name ${lib_name})

add_library(${target_name} STATIC ${SOURCE_FILES})

set_target_properties(${target_name}
        PROPERTIES
        OUTPUT_NAME_DEBUG ${target_name}d
        OUTPUT_NAME ${target_name}
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
        NetFwd.h
        SignalWatcher.h
        )

INSTALL(FILES ${HEAD_FILES} DESTINATION ${PROJECT_BINARY_DIR}/include/)

ADD_CUSTOM_COMMAND(TARGET ${target_name}
        POST_BUILD
        COMMAND make install
        )
