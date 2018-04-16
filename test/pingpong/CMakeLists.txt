cmake_minimum_required(VERSION 3.9)
project(PinPong)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_CXX_FLAGS_DEBUG  "$ENV{CXXFLAGS} -g -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE  "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(${PROJECT_SOURCE_DIR}/../../net_library/include)
link_libraries(debug ${PROJECT_SOURCE_DIR}/../../net_library/lib_static/libnetd.a)
link_libraries(optimized ${PROJECT_SOURCE_DIR}/../../net_library/lib_static/libnet.a)

link_libraries(pthread)

add_executable(pingpong_client client.cc)

add_executable(pingpong_server server.cc)

