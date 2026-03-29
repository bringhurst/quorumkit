# ---------------------------------------------------------------------------
# Protobuf code generation
#
# Proto generation lives here (not the top-level CMakeLists.txt) because
# add_custom_command OUTPUT rules are scoped to the directory they're
# defined in.  If defined in a different directory from add_library, CMake
# can't find the generated sources at configure time.
# ---------------------------------------------------------------------------
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTO_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_OUT_DIR}/braft)

file(GLOB BRAFT_PROTOS "${CMAKE_CURRENT_SOURCE_DIR}/braft/*.proto")

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

# ---------------------------------------------------------------------------
# braft library target
# ---------------------------------------------------------------------------

file(GLOB_RECURSE BRAFT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/braft/*.cpp")

add_library(braft STATIC
    ${BRAFT_SOURCES}
    ${PROTO_SRCS}
)

target_include_directories(braft
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>       # src/braft/foo.h via "braft/foo.h"
        $<BUILD_INTERFACE:${PROTO_OUT_DIR}>                   # generated braft/*.pb.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(braft
    PUBLIC
        BRPC_WITH_GLOG=0
        GFLAGS_NS=gflags
        BTHREAD_USE_FAST_PTHREAD_MUTEX
        __const__=__unused__
        _GNU_SOURCE
        USE_SYMBOLIZE
        NO_TCMALLOC
        __STDC_FORMAT_MACROS
        __STDC_LIMIT_MACROS
        __STDC_CONSTANT_MACROS
        __STRICT_ANSI__
        BRAFT_REVISION="${BRAFT_REVISION}"
)

if(BUILD_UNIT_TESTS)
    target_compile_definitions(braft PUBLIC UNIT_TEST)
endif()

target_compile_options(braft PRIVATE
    -pipe -Wall -W -fPIC -fstrict-aliasing
    -Wno-invalid-offsetof -Wno-unused-parameter
    -Wno-reserved-user-defined-literal
    -fno-omit-frame-pointer
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-O2>
)

# SSE4 only on x86
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    target_compile_options(braft PRIVATE -msse4 -msse4.2)
endif()

# macOS: check for clock_gettime (older SDKs may lack it)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(CheckFunctionExists)
    check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
    if(NOT HAVE_CLOCK_GETTIME)
        target_compile_definitions(braft PRIVATE NO_CLOCK_GETTIME_IN_MAC)
    endif()
endif()

target_link_libraries(braft
    PUBLIC
        protobuf::protobuf
        gflags::gflags
        leveldb::leveldb
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
        brpc::brpc
        Threads::Threads
)

# macOS-specific linker flags
#
# -Wl,-U,<symbol> tells the linker to allow <symbol> to be undefined at
# link time.  These symbols come from optional profiling libraries
# (gperftools, jemalloc) that brpc's static library references but that
# we don't link.  At runtime the symbols resolve to NULL or are never
# called because we compile with NO_TCMALLOC.
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_options(braft PUBLIC
        # gperftools symbols (tcmalloc / cpu profiler)
        "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
        "-Wl,-U,_ProfilerStart"
        "-Wl,-U,_ProfilerStop"
        # gperftools/libunwind stack trace — brpc's stack_trace_posix.cc
        # C++ mangled: int GetStackTrace(void**, int, int)
        "-Wl,-U,__Z13GetStackTracePPvii"
        # jemalloc profiler symbols — brpc's jemalloc_profiler.cpp
        "-Wl,-U,_mallctl"
        "-Wl,-U,_malloc_stats_print"
    )
    target_link_libraries(braft PUBLIC
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreData"
        "-framework CoreText"
        "-framework Security"
        "-framework Foundation"
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(braft PUBLIC dl rt)
endif()

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
install(TARGETS braft
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install public headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/braft/
    DESTINATION include/braft
    FILES_MATCHING PATTERN "*.h"
)

# Install generated protobuf headers
install(DIRECTORY ${PROTO_OUT_DIR}/braft/
    DESTINATION include/braft
    FILES_MATCHING PATTERN "*.h"
)
