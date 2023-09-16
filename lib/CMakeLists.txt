cmake_minimum_required(VERSION 3.5)

add_library(CubeLib drawSquareDoubles.c)
target_include_directories(CubeLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CubeLib PRIVATE m)

find_package(Threads REQUIRED)
target_link_libraries(CubeLib PRIVATE Threads::Threads)