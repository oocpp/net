cmake_minimum_required(VERSION 3.9)
project(http)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS_DEBUG  "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(../../net/)
link_libraries(/home/lg/Code/net/net/cmake-build-debug/libnet.a)


add_executable(http main.cpp)
target_link_libraries(http pthread)