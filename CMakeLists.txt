cmake_minimum_required(VERSION 3.5)
project(Cube)

# Set compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Add your source code directory
add_subdirectory(lib)

add_subdirectory(runner)
# Add your tests directory
add_subdirectory(tests)
