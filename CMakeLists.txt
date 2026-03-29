cmake_minimum_required(VERSION 3.15)
project(quorumkit VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
# Protobuf code generation
# ---------------------------------------------------------------------------
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROTO_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_OUT_DIR}/braft)

file(GLOB BRAFT_PROTOS "${CMAKE_CURRENT_SOURCE_DIR}/src/braft/*.proto")

set(PROTO_SRCS)
set(PROTO_HDRS)
foreach(PROTO ${BRAFT_PROTOS})
    get_filename_component(PROTO_WE ${PROTO} NAME_WE)
    set(OUT_CC "${PROTO_OUT_DIR}/braft/${PROTO_WE}.pb.cc")
    set(OUT_H  "${PROTO_OUT_DIR}/braft/${PROTO_WE}.pb.h")
    list(APPEND PROTO_SRCS ${OUT_CC})
    list(APPEND PROTO_HDRS ${OUT_H})
    add_custom_command(
        OUTPUT ${OUT_CC} ${OUT_H}
        COMMAND protobuf::protoc
            --cpp_out=${PROTO_OUT_DIR}
            --proto_path=${PROTO_SRC_DIR}
            ${PROTO}
        DEPENDS ${PROTO}
        COMMENT "Generating protobuf code for ${PROTO_WE}.proto"
        VERBATIM
    )
endforeach()

add_custom_target(braft_proto_gen DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

# ---------------------------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------------------------
add_subdirectory(src)

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
