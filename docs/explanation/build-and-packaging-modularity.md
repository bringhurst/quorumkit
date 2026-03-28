---
title: Build and packaging modularity
sidebar_position: 10
---

# Build and packaging modularity

The source tree defines what is public, what is internal, and where optional pieces plug in. The build system follows that structure -- it does not impose its own.

```text
cmake/       CMake modules, targets, install, and export logic
bazel/       Bazel wrappers and BUILD files
packaging/   package-manager metadata (Conan, vcpkg, etc.)
```

## CMake is the canonical build

CMake is the primary build system. The `cmake/` directory contains the target definitions, install rules, and export logic. Public headers get installed from `include/`. Internal sources are compiled but not installed.

## Bazel stays isolated

Bazel support lives entirely in `bazel/`. It wraps the same source tree but does not change the directory layout or introduce its own concepts into the rest of the repo. If you do not use Bazel, you can ignore the `bazel/` directory entirely.

## Packaging is separate

The `packaging/` directory holds metadata for package managers (Conan recipes, vcpkg port files, and so on). This keeps packaging concerns out of the main build logic. Adding support for a new package manager means adding files in `packaging/`, not restructuring the build.

## Why this matters

Build systems have a tendency to take over a repository -- dictating directory layout, header paths, and dependency structure. Keeping build and packaging tooling in their own directories means the source tree stays organized around the code, not around the tools used to compile it. If a better build system or package manager shows up, it can be added alongside the existing ones without rearranging everything.
