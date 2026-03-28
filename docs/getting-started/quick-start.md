---
title: Quick Start
sidebar_position: 1
---

# Quick Start

This page explains the fastest way to build the repository, enable tests, and orient yourself in the source tree.

## Build

QuorumKit uses CMake as the primary build entry point.

```sh
cmake -S . -B bld
cmake --build bld
```

## Build With Unit Tests

```sh
cmake -S . -B bld -DBUILD_UNIT_TESTS=ON
cmake --build bld
```

The test executables are emitted under the build tree. The test model is organized around three layers described throughout the docs: public API tests, internal subsystem tests, and deterministic simulation.

## Read The Repository In The Right Order

For a quick but accurate understanding of the project, use this sequence:

1. `README.md`
2. `docs/index.md`
3. `docs/concepts/mental-model.md`
4. `docs/api/overview.md`
5. `docs/architecture/index.md`

## Repository Contract

- `include/quorumkit` defines the canonical public API surface.
- `include/braft` defines the supported compatibility surface.
- `src/internal` contains implementation details.
- `test/public` contains public API contract tests.
- `test/internal` contains subsystem tests.
- `test/simulation` contains deterministic cluster and fault tests.

These directories define the contract of the repository. Public code is obvious by placement, and internal code is free to change behind those boundaries.

## Build Philosophy

The source tree owns the architecture. CMake, Bazel, and package-manager integrations consume that architecture instead of defining it.

This keeps the repository usable through different distribution systems without changing public header layout or internal boundaries.
