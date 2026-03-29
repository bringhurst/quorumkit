cmake_minimum_required(VERSION 3.15)
project(quorumkit VERSION 0.1.0 LANGUAGES C CXX)

# C++ standard is set by Conan's CMakeToolchain (gnu17 from the default
# profile).  Do NOT override it here — GTest 1.14 requires C++14 minimum,
# and the braft library code compiles cleanly under C++17.

option(BUILD_UNIT_TESTS "Build unit tests" OFF)

# ---------------------------------------------------------------------------
# Dependencies (all provided by Conan)
# ---------------------------------------------------------------------------
find_package(Protobuf REQUIRED)
find_package(gflags REQUIRED)
find_package(leveldb REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(brpc REQUIRED)
find_package(Threads REQUIRED)

if(BUILD_UNIT_TESTS)
    find_package(GTest REQUIRED)
endif()

# ---------------------------------------------------------------------------
# Git revision stamp
# ---------------------------------------------------------------------------
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE BRAFT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT BRAFT_REVISION)
    set(BRAFT_REVISION "unknown")
endif()

# ---------------------------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------------------------
add_subdirectory(src)

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
