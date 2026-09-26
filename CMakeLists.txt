cmake_minimum_required(VERSION 3.21)
project(WireZero VERSION 0.1.0 LANGUAGES CXX)

# Enforce C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /wx)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion)
endif()

# Library Target
add_library(wirezero 
    src/wirezero.cpp
)

target_include_directories(wirezero
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Enable Testing
enable_testing()
option(WIREZERO_BUILD_TESTS "Build test suite" ON)

if(WIREZERO_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/runtime settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()