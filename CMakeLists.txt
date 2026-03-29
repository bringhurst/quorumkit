cmake_minimum_required(VERSION 3.15)
project(quorumkit VERSION 0.1.0 LANGUAGES C CXX)

# C++ standard is set by Conan's CMakeToolchain (gnu17 from the default
# profile).  Do NOT override it here — GTest 1.14 requires C++14 minimum,
# and the braft library code compiles cleanly under C++17.

option(BUILD_UNIT_TESTS "Build unit tests" OFF)
option(QUORUMKIT_SANITIZER_BUILD "Adjust test settings for sanitizer runs" OFF)
set(QUORUMKIT_TEST_TIMEOUT_SECONDS "120" CACHE STRING "CTest timeout for regular builds")
set(QUORUMKIT_SANITIZER_TEST_TIMEOUT_SECONDS "300" CACHE STRING "CTest timeout for sanitizer builds")

# Sanitizer compiler and linker flags come from Conan profiles so dependencies
# (especially brpc) are built consistently too.  This option only tunes the
# test runtime settings inside the main CMake project.
if(QUORUMKIT_SANITIZER_BUILD)
    set(QUORUMKIT_EFFECTIVE_TEST_TIMEOUT_SECONDS "${QUORUMKIT_SANITIZER_TEST_TIMEOUT_SECONDS}")
else()
    set(QUORUMKIT_EFFECTIVE_TEST_TIMEOUT_SECONDS "${QUORUMKIT_TEST_TIMEOUT_SECONDS}")
endif()

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
