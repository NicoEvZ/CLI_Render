cmake_minimum_required(VERSION 3.5)
add_executable(Cube runner2.c)
target_include_directories(Cube PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Cube PRIVATE CubeLib m)
