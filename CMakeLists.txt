cmake_minimum_required(VERSION 3.10)
project(ALF)

# Determine Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Build options are: Debug, Release, RelWithDebInfo, MinSizeRel, TestDebug, TestRel"
        FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(ALF 
    PUBLIC 
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(libalf)
