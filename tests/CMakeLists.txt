cmake_minimum_required(VERSION 3.10)
file(GLOB_RECURSE alf_test_sources *)

set(CMAKE_CXX_STANDARD 11)  # GoogleTest requires at least C++11

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(alf_tests ${alf_test_sources})

target_link_libraries(alf_tests libalf)
target_link_libraries(alf_tests gtest_main)

include(GoogleTest)
gtest_discover_tests(alf_tests)
