#!/bin/bash

resolve_example_binary() {
    local example_dir="$1"
    local example_name="$2"
    local binary_name="$3"
    local repo_root
    local candidate

    repo_root="$(cd "${example_dir}/../.." && pwd)"

    for candidate in \
        "${example_dir}/${binary_name}" \
        "${repo_root}/build/build/Release/examples/${example_name}/${binary_name}" \
        "${repo_root}/build/build/RelWithDebInfo/examples/${example_name}/${binary_name}" \
        "${repo_root}/build/build/Debug/examples/${example_name}/${binary_name}"
    do
        if [[ -x "${candidate}" ]]; then
            printf '%s\n' "${candidate}"
            return 0
        fi
    done

    return 1
}
