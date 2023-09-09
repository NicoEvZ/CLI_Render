cmake_minimum_required(VERSION 3.0)
project(UnitTests)

include(CTest)

set (CubeLib_DIR "${CMAKE_SOURCE_DIR}/../src")
add_subdirectory(${CubeLib_DIR} CubeLib_build)
add_executable(UnitTests tests.c)
target_include_directories(UnitTests PRIVATE ${CubeLib_DIR})
target_compile_definitions(UnitTests PUBLIC TESTING)
# Include(FetchContent)

# FetchContent_Declare(
#         CMocka
#         GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
#         GIT_TAG    cmocka-1.1.5
# )

# FetchContent_MakeAvailable(CMocka)


target_link_libraries(UnitTests PRIVATE CubeLib)
