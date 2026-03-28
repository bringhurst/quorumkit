---
title: Repository map
sidebar_position: 3
---

# Repository map

Use this page when you want the short factual version of the repository layout.

```text
docs/         documentation source
include/      public API headers
src/          implementation
test/         tests
example/      runnable sample applications
cmake/        canonical build and install logic
bazel/        Bazel integration
packaging/    package-manager integration
```

Inside the code tree:

```text
include/quorumkit   canonical public API
include/braft       compatibility API
src/quorumkit       public-layer implementation
src/braft_compat    compatibility adapters
src/internal        internal implementation
```

For the reasoning behind that split, read [Repository layout](../explanation/repository-layout).
