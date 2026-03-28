---
title: Repository Tour
sidebar_position: 2
---

# Repository Tour

The repository is laid out so that a reader can see public boundaries and internal boundaries immediately.

## Top-Level View

```text
README.md
docs/
include/
src/
test/
example/
tools/
cmake/
bazel/
packaging/
```

## Public Surfaces

The public surfaces are the directories that consumers are meant to include and depend on directly.

```text
include/
  quorumkit/
  braft/
```

`include/quorumkit` is the canonical API.

`include/braft` is the compatibility API.

If a header is not under one of these roots, it is not part of the supported installed contract.

## Implementation Surfaces

The implementation surface is split by role.

```text
src/
  quorumkit/
  braft_compat/
  internal/
```

- `src/quorumkit` contains canonical public-layer implementation glue.
- `src/braft_compat` contains compatibility-only adapters.
- `src/internal` contains the consensus engine, runtime adapters, storage adapters, transport adapters, and implementation-only helpers.

## Tests

The test tree mirrors the architecture.

```text
test/
  public/
    quorumkit/
    braft_compat/
  internal/
  simulation/
```

This makes the testing strategy legible from the tree itself.

## Examples And Tools

`example/` contains sample applications.

`tools/` contains command-line utilities and operational helpers.

These directories are not substitutes for the public header documentation. They show how the APIs are used in complete programs.

## Repository Contract

The source tree makes public and internal boundaries visible by placement.

Public headers live under explicit include roots, compatibility code lives under its own adapter directory, and implementation detail stays under internal directories. The documentation uses that boundary as the main organizing principle.
