---
title: Testing architecture
sidebar_position: 9
---

# Testing architecture

The test tree mirrors the source tree, with three distinct areas:

```text
test/
  public/
    quorumkit/       tests against the QuorumKit public API
    braft_compat/    tests against the braft compatibility API
  internal/
    core/            unit tests for the Raft state machine
    runtime/         tests for clocks, schedulers, random sources
    transport/       tests for message delivery
    storage/         tests for log, metadata, and snapshot backends
    snapshot/        tests for snapshot save/load
  simulation/
    cluster/         multi-node cluster simulations
    fault/           fault injection scenarios
    workload/        workload-driven tests
```

## Public API tests

Tests under `test/public/` use only the installed headers (`include/quorumkit/` and `include/braft/`). They verify that the library behaves correctly from a user's perspective: creating nodes, applying operations, performing snapshots, running admin commands. If an internal refactor breaks one of these tests, the public contract has changed.

## Internal tests

Tests under `test/internal/` reach into `src/internal/` and test individual modules in isolation. These are the usual unit tests: feed a component specific inputs, check specific outputs. They run fast and catch regressions early, but they are allowed to break when internals are restructured.

## Simulation tests

Tests under `test/simulation/` are the most interesting. They wire up multiple Raft nodes using the in-memory transport, deterministic clock, and in-memory storage. Everything runs in a single process with no real I/O. The test harness controls exactly when messages are delivered, when timers fire, and when storage operations complete.

This makes it possible to write tests for scenarios that are difficult to reproduce reliably in a real cluster: network partitions, asymmetric connectivity, message reordering, slow disks, and clock skew. Because the simulation is fully deterministic, a failing test case produces the same behavior every time you run it.
