---
title: Navigate the repository
sidebar_position: 2
---

# Navigate the repository

The top-level directories:

```text
src/         library source, headers, and proto files
tests/       unit tests
examples/    runnable sample applications
contrib/     local Conan recipes for deps not on Conan Center
website/     documentation site (this site)
```

Inside the code:

- `src/braft/` -- the library source. Headers, implementation files, and 8 protobuf definitions all live here. Public headers are in `src/braft/*.h` (no separate `include/` directory yet).
- `tests/` -- 23 unit test files (`test_*.cpp`), plus helper headers (`util.h`, `sstream_workaround.h`).
- `examples/counter/`, `examples/atomic/`, `examples/block/` -- demo applications. Build them from the repo root with `-DBUILD_EXAMPLES=ON`.
- `contrib/brpc/` -- local Conan recipe for brpc 1.16.0, which is not available on Conan Center.
- `conanfile.py` -- the project's Conan recipe, declaring all dependencies.

For the reasoning behind this layout, see [Repository layout](../explanation/repository-layout).
