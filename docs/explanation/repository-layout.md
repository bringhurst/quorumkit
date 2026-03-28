---
title: Repository layout
sidebar_position: 2
---

# Repository layout

The directory tree is organized so you can tell at a glance whether something is a public contract or an internal detail.

## The tree

```text
include/
  quorumkit/        canonical public API
  braft/            compatibility API

src/
  quorumkit/        implements the QuorumKit headers
  braft_compat/     implements the braft headers (calls into QuorumKit)
  internal/         everything else: core, runtime, transport, storage

test/
  public/
    quorumkit/      tests against the QuorumKit API
    braft_compat/   tests against the braft API
  internal/         tests for internal modules
  simulation/       deterministic simulation tests

examples/
  quorumkit/        examples using the QuorumKit API
  braft_compat/     examples using the braft API

tools/
  quorumkit/
  braft/

cmake/
bazel/
packaging/
docs/
```

## Why this split matters

When implementation headers get mixed in with public ones, every internal refactor risks breaking users. This layout makes it hard to do that by accident: if a header is in `include/quorumkit/`, it is a public contract; if it is in `src/internal/`, it is free to change.

The braft compatibility headers get their own directory under `include/braft/` rather than being mixed into the QuorumKit tree. That keeps the two surfaces distinct -- you can see exactly which headers exist for compatibility and which ones are the main API.

## Dependency direction

```mermaid
flowchart LR
    B[include/braft] --> Q[include/quorumkit]
    Q --> SQ[src/quorumkit]
    SQ --> I[src/internal]
    BC[src/braft_compat] --> Q
    BC --> I
```

Dependencies point inward. The braft headers depend on the QuorumKit headers. The QuorumKit implementation depends on the internals. Internal code never depends on anything public. This means you can restructure `src/internal/` without touching either public surface.
