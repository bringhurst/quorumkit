# ---------------------------------------------------------------------------
# braft library target
# ---------------------------------------------------------------------------

file(GLOB_RECURSE BRAFT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/braft/*.cpp")

add_library(braft STATIC
    ${BRAFT_SOURCES}
    ${PROTO_SRCS}
)

add_dependencies(braft braft_proto_gen)

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
else()
    target_compile_definitions(braft PRIVATE NDEBUG)
endif()

target_compile_options(braft PRIVATE
    -O2 -pipe -Wall -W -fPIC -fstrict-aliasing
    -Wno-invalid-offsetof -Wno-unused-parameter
    -Wno-reserved-user-defined-literal
    -fno-omit-frame-pointer
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
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_options(braft PUBLIC
        "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
        "-Wl,-U,_ProfilerStart"
        "-Wl,-U,_ProfilerStop"
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
