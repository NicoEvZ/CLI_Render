Include(FetchContent)

FetchContent_Declare(
        CMocka
        GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
        GIT_TAG    cmocka-1.1.5
)

FetchContent_MakeAvailable(CMocka)

add_executable(tests tests.c)

target_link_libraries(tests CubeLib cmocka)
