cmake_minimum_required(VERSION 3.21)


add_library(thirdparty-cJSON source/cJSON.c)

target_include_directories(
    thirdparty-cJSON
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/private
    )

target_include_directories(
        thirdparty-cJSON
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})