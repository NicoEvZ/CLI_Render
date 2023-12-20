cmake_minimum_required(VERSION 3.5)
add_executable(CLI_Render runner.c)
target_include_directories(CLI_Render PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CLI_Render PRIVATE CLI_RenderLib m thirdparty-cJSON)
