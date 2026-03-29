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

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
