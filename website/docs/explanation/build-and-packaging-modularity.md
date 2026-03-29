---
title: Build and packaging modularity
sidebar_position: 10
---

# Build and packaging modularity

The source tree defines what is public, what is internal, and where optional pieces plug in. The build system follows that structure -- it does not impose its own.

```text
conanfile.py          Conan 2 recipe for QuorumKit
contrib/brpc/         local Conan recipe for brpc (not on Conan Center)
CMakeLists.txt        top-level CMake project
src/CMakeLists.txt    library target, protobuf generation
test/CMakeLists.txt   test targets with isolation and resource locks
```

## Conan 2 manages dependencies

All C++ dependencies are declared in `conanfile.py` and resolved by Conan 2. The dependency list: protobuf 3.21.12, gflags, leveldb, openssl, zlib, brpc 1.11.0, and gtest (for tests). Conan generates a CMake toolchain and presets, so the build uses `cmake --preset conan-release` instead of manual `-D` flags.

Dependencies not available on Conan Center (currently just brpc) have local recipes under `contrib/`. This keeps the build self-contained without vendoring source trees.

## CMake is the build system

CMake is the only build system. Protobuf code generation, the static library target, test binaries, and install rules all live in CMakeLists.txt files. The top-level project delegates to `src/` and `test/` subdirectories.

## Why this matters

Build systems have a tendency to take over a repository -- dictating directory layout, header paths, and dependency structure. By using Conan for dependency resolution and keeping CMake focused on compilation, neither tool dictates the source layout. If a better package manager shows up, replacing `conanfile.py` and `contrib/` is all that's needed.
