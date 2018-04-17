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
        )

set(target_name ${lib_name})

set(NET_LIBRARY 1)  #设置为0，用来与main.cpp编译成可执行程序，便于调试

if(NET_LIBRARY)
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
            )

    INSTALL(FILES ${HEAD_FILES} DESTINATION ${PROJECT_BINARY_DIR}/include/)
    ADD_CUSTOM_COMMAND(TARGET ${target_name}
            POST_BUILD
            COMMAND make install
    )

else()
    add_executable(net main.cpp ${SOURCE_FILES})
endif()

