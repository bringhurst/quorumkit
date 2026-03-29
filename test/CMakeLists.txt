# ---------------------------------------------------------------------------
# Test targets
# ---------------------------------------------------------------------------

file(GLOB TEST_SRCS "test_*.cpp")

foreach(TEST_SRC ${TEST_SRCS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)

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
            ${PROTO_OUT_DIR}                     # for generated pb headers
    )

    # Access private/protected members for white-box testing
    target_compile_options(${TEST_NAME} PRIVATE
        -Dprivate=public
        -Dprotected=public
        -include sstream_workaround.h
        -O2 -pipe -Wall -W -fPIC -fstrict-aliasing
        -Wno-invalid-offsetof -Wno-unused-parameter
        -Wno-unused-result
        -fno-omit-frame-pointer
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
        TIMEOUT 120
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

# Ports 5006-5010: test_node, test_leader_lease, test_cli
foreach(T test_node test_leader_lease test_cli)
    if(TARGET ${T})
        set_tests_properties(${T} PROPERTIES RESOURCE_LOCK "ports_5006_5010")
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
