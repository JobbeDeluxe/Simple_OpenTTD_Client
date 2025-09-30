cmake_minimum_required(VERSION 3.21)
project(SimpleOpenTTDClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SOTC_BUILD_TESTS "Build unit tests" OFF)

add_library(project_warnings INTERFACE)
if(MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4
        /permissive-
        /WX-)
else()
    target_compile_options(project_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic)
endif()

add_subdirectory(src)

if(SOTC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
