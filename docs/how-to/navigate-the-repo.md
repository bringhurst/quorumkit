---
title: Navigate the repository
sidebar_position: 2
---

# Navigate the repository

The top-level directories:

```text
include/     public headers (installed with the library)
src/         implementation
test/        tests
example/     runnable sample applications
docs/        documentation source (this site)
cmake/       CMake modules and install logic
bazel/       Bazel integration
packaging/   package-manager metadata
```

Inside the code:

- `include/quorumkit/` -- the canonical public API. New code should use these headers.
- `include/braft/` -- the compatibility API. Existing braft code can keep using these.
- `src/quorumkit/` -- implementation behind the QuorumKit headers.
- `src/braft_compat/` -- thin adapters that make the braft headers work on top of the QuorumKit internals.
- `src/internal/` -- everything else: the Raft core, runtime adapters, transport, storage, and snapshot logic.

The split is there so you can tell at a glance whether something is a public contract or an internal detail. For the reasoning behind this layout, see [Repository layout](../explanation/repository-layout).
