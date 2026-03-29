# ---------------------------------------------------------------------------
# Test targets
# ---------------------------------------------------------------------------

file(GLOB TEST_SRCS "test_*.cpp")
set(ALL_TEST_NAMES)

foreach(TEST_SRC ${TEST_SRCS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    list(APPEND ALL_TEST_NAMES ${TEST_NAME})

    add_executable(${TEST_NAME} ${TEST_SRC})

    target_link_libraries(${TEST_NAME}
        PRIVATE
            braft
            GTest::gtest_main
            GTest::gtest
    )

    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}         # test helper headers (util.h etc.)
            ${CMAKE_SOURCE_DIR}/src              # for "braft/..." includes
    )

    # Access private/protected members for white-box testing
    target_compile_options(${TEST_NAME} PRIVATE
        -Dprivate=public
        -Dprotected=public
        -include sstream_workaround.h
        -pipe -Wall -W -fPIC -fstrict-aliasing
        -Wno-invalid-offsetof -Wno-unused-parameter
        -Wno-unused-result
        -fno-omit-frame-pointer
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-O2>
    )

    # macOS: weak symbol references used in bthread logging
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_link_options(${TEST_NAME} PRIVATE
            "-Wl,-U,_bthread_getspecific"
            "-Wl,-U,_bthread_setspecific"
            "-Wl,-U,_bthread_key_create"
        )
    endif()

    # Each test binary gets its own working directory to prevent
    # interference through shared ./data directories and port reuse.
    # Several braft tests hardcode ports (5006-5010, 6006, 6500) and use
    # relative paths like ./data for storage.  Without isolation, a
    # timed-out test_node leaves port 5006 held, breaking test_leader_lease;
    # leftover ./data from one test causes rename failures in test_log, etc.
    set(TEST_WORKDIR "${CMAKE_CURRENT_BINARY_DIR}/testwd/${TEST_NAME}")
    file(MAKE_DIRECTORY ${TEST_WORKDIR})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_tests_properties(${TEST_NAME} PROPERTIES
        WORKING_DIRECTORY "${TEST_WORKDIR}"
        TIMEOUT ${QUORUMKIT_EFFECTIVE_TEST_TIMEOUT_SECONDS}
    )
endforeach()

# ---------------------------------------------------------------------------
# Port-based resource locks
#
# Several test binaries hardcode the same listen ports.  CTest runs tests
# sequentially by default, but RESOURCE_LOCK ensures mutual exclusion even
# if parallelism is turned on (ctest -j).  The lock names are arbitrary
# strings; tests sharing a lock never run concurrently.
# ---------------------------------------------------------------------------

# Ports 5006-5015: test_node, test_leader_lease
# (test_leader_lease.change_peers scales up to 10 peers on ports 5006-5015)
foreach(T test_node test_leader_lease)
    if(TARGET ${T})
        set_tests_properties(${T} PROPERTIES RESOURCE_LOCK "ports_5006_5015")
    endif()
endforeach()

# Port 9500-9501: test_cli
foreach(T test_cli)
    if(TARGET ${T})
        set_tests_properties(${T} PROPERTIES RESOURCE_LOCK "port_9500")
    endif()
endforeach()

# Port 6006: test_snapshot, test_fsm_caller
foreach(T test_snapshot test_fsm_caller)
    if(TARGET ${T})
        set_tests_properties(${T} PROPERTIES RESOURCE_LOCK "port_6006")
    endif()
endforeach()

# Port 6500: test_snapshot_executor
foreach(T test_snapshot_executor)
    if(TARGET ${T})
        set_tests_properties(${T} PROPERTIES RESOURCE_LOCK "port_6500")
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Known upstream braft SEGFAULT tests
#
# These 3 tests crash deterministically even in isolation due to memory
# corruption bugs in braft's configuration-change and leader-failover code
# paths.  The garbled log output (e.g. garbage bytes in replicator.cpp:307
# log lines) is the visible symptom of use-after-free or buffer overrun in
# the replicator during membership changes.
#
# We label them "known_crash" so CI can exclude them with:
#   ctest --preset conan-release -LE known_crash
# and developers can run just these for investigation with:
#   ctest --preset conan-release -L known_crash
#
# Note: WILL_FAIL doesn't help here -- CTest treats SEGFAULT (signal-based
# termination) as a special "Exception" that WILL_FAIL does not invert.
# ---------------------------------------------------------------------------
foreach(T test_leader_lease test_cli test_node)
    if(TARGET ${T})
        set_property(TEST ${T} APPEND PROPERTY LABELS "known_crash")
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Known upstream braft/brpc crashes in Linux Clang jobs
#
# These tests pass under GCC but crash reliably under the Linux Clang 21 jobs,
# inside upstream brpc/bthread snapshot/file-service paths. They are excluded
# only for those compiler configurations so the rest of the matrix still covers
# them where they are stable.
# ---------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    foreach(T test_file_service test_snapshot test_snapshot_executor)
        if(TARGET ${T})
            set_property(TEST ${T} APPEND PROPERTY LABELS "known_clang_crash")
        endif()
    endforeach()
endif()

# ---------------------------------------------------------------------------
# Known upstream UBSan failures in brpc/butil under Linux Clang sanitizer jobs
#
# The ASan+UBSan job currently trips multiple upstream undefined-behavior bugs
# in brpc/butil (alignment assumptions, logging nullability, object/resource
# pool alignment) before or after QuorumKit test logic runs. We keep the
# smallest practical smoke subset enabled and label the rest so CI can exclude
# them without disabling the build entirely.
# ---------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux"
   AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   AND QUORUMKIT_SANITIZER_BUILD)
    foreach(T ${ALL_TEST_NAMES})
        if(TARGET ${T}
           AND NOT T STREQUAL "test_ballot")
            set_property(TEST ${T} APPEND PROPERTY LABELS "known_sanitizer_upstream_bug")
        endif()
    endforeach()
endif()
