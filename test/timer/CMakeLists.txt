cmake_minimum_required(VERSION 3.9)
project(test1)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)
include_directories(../../net/)
link_libraries(/home/lg/Code/net/net/cmake-build-debug/libnet.a)

add_executable(test1 main.cpp)
