---
title: Repository map
sidebar_position: 3
---

# Repository map

## Top level

```text
include/      public headers (installed with the library)
src/          implementation source
test/         tests
example/      runnable sample applications
docs/         documentation source (this site)
cmake/        CMake modules, targets, install logic
bazel/        Bazel integration
packaging/    package-manager metadata
```

## Inside the code

```text
include/quorumkit/   canonical public API
include/braft/       compatibility API

src/quorumkit/       implementation of the QuorumKit headers
src/braft_compat/    compatibility adapters for the braft headers
src/internal/        Raft core, runtime, transport, storage, snapshots

test/public/         tests against the public APIs
test/internal/       tests for internal modules
test/simulation/     deterministic simulation tests
```

For the reasoning behind this split, see [Repository layout](../explanation/repository-layout).
