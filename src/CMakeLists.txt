cmake_minimum_required(VERSION 3.26)
add_executable(Cube drawSquareDoubles.c)
target_link_libraries(Cube PRIVATE m)

find_package(Threads REQUIRED)
target_link_libraries(Cube PRIVATE Threads::Threads)

add_library(CubeLib drawSquareDoubles.c)
target_link_libraries(CubeLib PRIVATE m)
target_link_libraries(CubeLib PRIVATE Threads::Threads)