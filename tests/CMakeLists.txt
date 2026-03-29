# ---------------------------------------------------------------------------
# Test suite organization
#
# tests/public/   - public-facing behavior we intend to turn into contracts
# tests/internal/ - QuorumKit implementation tests (white-box is allowed)
# tests/legacy/   - inherited upstream-style coverage that still gives us broad
#                   regression protection, but does not define the public spec
# tests/support/  - shared test helpers only
# ---------------------------------------------------------------------------

include(CMakeParseArguments)

set(QUORUMKIT_TESTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(QUORUMKIT_TEST_BINARY_ROOT ${CMAKE_CURRENT_BINARY_DIR})

function(quorumkit_add_test)
    set(options WHITEBOX)
    set(oneValueArgs NAME SCOPE DOMAIN RESOURCE_LOCK)
    set(multiValueArgs SOURCES LABELS)
    cmake_parse_arguments(QT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT QT_NAME)
        message(FATAL_ERROR "quorumkit_add_test requires NAME")
    endif()
    if(NOT QT_SCOPE)
        message(FATAL_ERROR "quorumkit_add_test requires SCOPE")
    endif()
    if(NOT QT_SOURCES)
        message(FATAL_ERROR "quorumkit_add_test requires SOURCES")
    endif()

    add_executable(${QT_NAME} ${QT_SOURCES})

    target_link_libraries(${QT_NAME}
        PRIVATE
            braft
            GTest::gtest_main
            GTest::gtest
    )

    target_include_directories(${QT_NAME}
        PRIVATE
            ${QUORUMKIT_TESTS_ROOT}
            ${QUORUMKIT_TESTS_ROOT}/support
            ${CMAKE_SOURCE_DIR}/src
    )

    target_compile_options(${QT_NAME} PRIVATE
        -pipe -Wall -W -fPIC -fstrict-aliasing
        -Wno-invalid-offsetof -Wno-unused-parameter
        -Wno-unused-result
        -fno-omit-frame-pointer
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-O2>
    )

    if(QT_WHITEBOX)
        target_compile_options(${QT_NAME} PRIVATE
            -Dprivate=public
            -Dprotected=public
            -include support/sstream_workaround.h
        )
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_link_options(${QT_NAME} PRIVATE
            "-Wl,-U,_bthread_getspecific"
            "-Wl,-U,_bthread_setspecific"
            "-Wl,-U,_bthread_key_create"
        )
    endif()

    set(test_labels ${QT_SCOPE})
    if(QT_WHITEBOX)
        list(APPEND test_labels whitebox)
    else()
        list(APPEND test_labels public_api)
    endif()
    if(QT_DOMAIN)
        list(APPEND test_labels ${QT_DOMAIN})
    endif()
    list(APPEND test_labels ${QT_LABELS})
    list(REMOVE_DUPLICATES test_labels)

    set(test_workdir "${QUORUMKIT_TEST_BINARY_ROOT}/testwd/${QT_NAME}")
    file(MAKE_DIRECTORY ${test_workdir})

    add_test(NAME ${QT_NAME} COMMAND $<TARGET_FILE:${QT_NAME}>)
    set_tests_properties(${QT_NAME} PROPERTIES
        WORKING_DIRECTORY "${test_workdir}"
        TIMEOUT ${QUORUMKIT_EFFECTIVE_TEST_TIMEOUT_SECONDS}
        LABELS "${test_labels}"
    )

    if(QT_RESOURCE_LOCK)
        set_tests_properties(${QT_NAME} PROPERTIES RESOURCE_LOCK "${QT_RESOURCE_LOCK}")
    endif()

    if(QT_NAME STREQUAL "test_leader_lease"
       OR QT_NAME STREQUAL "test_cli"
       OR QT_NAME STREQUAL "test_node")
        set_property(TEST ${QT_NAME} APPEND PROPERTY LABELS "known_crash")
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux"
       AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
       AND (QT_NAME STREQUAL "test_file_service"
            OR QT_NAME STREQUAL "test_snapshot"
            OR QT_NAME STREQUAL "test_snapshot_executor"))
        set_property(TEST ${QT_NAME} APPEND PROPERTY LABELS "known_clang_crash")
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux"
       AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
       AND QUORUMKIT_SANITIZER_BUILD
       AND NOT QT_NAME STREQUAL "test_ballot")
        set_property(TEST ${QT_NAME} APPEND PROPERTY LABELS "known_sanitizer_upstream_bug")
    endif()

    set_property(GLOBAL APPEND PROPERTY QUORUMKIT_ALL_TEST_NAMES ${QT_NAME})
endfunction()

function(quorumkit_add_public_test)
    quorumkit_add_test(SCOPE public ${ARGN})
endfunction()

function(quorumkit_add_internal_test)
    quorumkit_add_test(SCOPE internal WHITEBOX ${ARGN})
endfunction()

function(quorumkit_add_legacy_test)
    quorumkit_add_test(SCOPE legacy WHITEBOX ${ARGN})
endfunction()

add_subdirectory(public)
add_subdirectory(internal)
add_subdirectory(legacy)
