cmake_minimum_required(VERSION 3.10)
project(Cube)


add_executable(Cube drawSquareDoubles.c)
target_link_libraries(Cube PRIVATE m)

find_package(Threads REQUIRED)
target_link_libraries(Cube PRIVATE Threads::Threads)
