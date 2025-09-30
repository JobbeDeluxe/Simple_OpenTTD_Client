cmake_minimum_required(VERSION 3.21)

project(SimpleOpenTTDClient VERSION 0.1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(SOTC_BUILD_TESTS "Build unit tests" OFF)
option(SOTC_ENABLE_IPO "Enable interprocedural optimisation when supported" OFF)
option(SOTC_USE_OPENSSL "Link against OpenSSL for TLS support" ON)

include(GNUInstallDirs)
include(CMakePrintHelpers OPTIONAL)
include(CheckCXXCompilerFlag OPTIONAL)
include(SOTCCompilerWarnings)
include(SOTCDependencies)

add_library(project_warnings INTERFACE)
sotc_set_project_warnings(project_warnings)

add_library(project_options INTERFACE)

if(MSVC)
    target_compile_definitions(project_options INTERFACE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    target_compile_options(project_options INTERFACE
        /permissive-
        /Zc:__cplusplus
        /Zc:preprocessor
    )
else()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(project_options INTERFACE -fcolor-diagnostics)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(project_options INTERFACE -fdiagnostics-color=always)
    endif()

    find_package(Threads REQUIRED)
    target_link_libraries(project_options INTERFACE Threads::Threads)
endif()

add_library(project_dependencies INTERFACE)
sotc_configure_dependencies(project_dependencies)

if(SOTC_ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(WARNING "IPO is not supported by the current toolchain: ${ipo_output}")
    endif()
endif()

add_subdirectory(src)

if(SOTC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
