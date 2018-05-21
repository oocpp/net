cmake_minimum_required(VERSION 2.8)
project(net-library)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wno-unused-parameter
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        )

SET(CMAKE_CXX_FLAGS_DEBUG  "$ENV{CXXFLAGS} -O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE  "$ENV{CXXFLAGS} -O3")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

SET(DEPENDENT_LIBRARIES pthread)

SET(lib_name "net")

add_subdirectory (net)
add_subdirectory (test)
