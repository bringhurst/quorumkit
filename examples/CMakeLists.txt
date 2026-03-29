# ---------------------------------------------------------------------------
# Example applications
# ---------------------------------------------------------------------------

# Examples reuse the main braft target and Conan-provided dependencies from the
# top-level build.  They are intentionally opt-in via BUILD_EXAMPLES so the
# default developer workflow stays focused on library/test iteration.

function(quorumkit_add_example example_name proto_file)
    protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto_file})

    foreach(program_name ${ARGN})
        add_executable(${example_name}_${program_name}
            ${program_name}.cpp
            ${PROTO_SRCS}
            ${PROTO_HDRS}
        )

        target_include_directories(${example_name}_${program_name}
            PRIVATE
                ${CMAKE_CURRENT_BINARY_DIR}
        )

        target_link_libraries(${example_name}_${program_name}
            PRIVATE
                braft
        )

        target_compile_features(${example_name}_${program_name}
            PRIVATE
                cxx_std_11
        )
    endforeach()
endfunction()

add_subdirectory(counter)
add_subdirectory(atomic)
add_subdirectory(block)
