cmake_minimum_required(VERSION 3.5)

add_library(CLI_RenderLib draw.c cJSON.c)
target_include_directories(CLI_RenderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CLI_RenderLib PRIVATE m)

find_package(Threads REQUIRED)
target_link_libraries(CLI_RenderLib PRIVATE Threads::Threads)