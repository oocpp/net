cmake_minimum_required(VERSION 3.9)
project(ChatClient)

set(CMAKE_CXX_STANDARD 14)


include_directories(../../net/)
link_libraries(/home/lg/Code/net/net/cmake-build-debug/libnet.a)
add_executable(ChatClient main.cpp)

target_link_libraries(ChatClient pthread)
