include_directories(${PROJECT_BINARY_DIR}/include)

link_libraries(${lib_name})
link_libraries(${DEPENDENT_LIBRARIES})

add_subdirectory (chat)
add_subdirectory (http)
add_subdirectory (pingpong)
add_subdirectory (timer)
add_subdirectory (echo)
add_subdirectory (signal)

