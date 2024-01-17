cmake_minimum_required(VERSION 3.5)
project(UnitTests)

include(CTest)

add_executable(UnitTests tests.c)

Include(FetchContent)

FetchContent_Declare(
        CMocka
        GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
        GIT_TAG    cmocka-1.1.5
)

FetchContent_MakeAvailable(CMocka)

target_link_libraries(UnitTests PRIVATE CLI_RenderLib cmocka)

IF (WIN32)
# Copy cmocka.dll to the executable directory after build
add_custom_command(
    TARGET UnitTests
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/_deps/cmocka-build/src/cmocka.dll
            ${CMAKE_BINARY_DIR}/tests/
)
ENDIF()