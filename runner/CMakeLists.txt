cmake_minimum_required(VERSION 3.0)
add_executable(Cube drawSquareDoubles.c)
target_include_directories(Cube PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Cube PRIVATE m)

find_package(Threads REQUIRED)
target_link_libraries(Cube PRIVATE Threads::Threads)

add_library(CubeLib drawSquareDoubles.c)
target_include_directories(CubeLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CubeLib PRIVATE m)
target_link_libraries(CubeLib PRIVATE Threads::Threads)