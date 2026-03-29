---
title: Internal architecture
sidebar_position: 5
---

# Internal architecture

Everything under `src/internal/` is implementation. None of it is installed as public headers. It can change between releases without notice.

## The layout

```text
src/internal/
  core/        the deterministic Raft state machine
  runtime/     clocks, schedulers, execution adapters
  rpc/         message transport (currently brpc-based)
  storage/     log, metadata, and snapshot backends
  snapshot/    snapshot save/load machinery
  proto/       protobuf definitions
  base/        shared utilities
```

## What `core/` does

`core/` is the heart of the library. It contains the Raft state machine: election logic, log replication, commitment tracking, and application ordering. The code in `core/` does not call the clock, open sockets, or touch the filesystem. It receives inputs (messages, timer ticks, client requests) and produces outputs (messages to send, log entries to persist, state transitions to apply). This is what makes it possible to run the core under a deterministic test harness with simulated time and faults.

## What the adapters do

The rest of `src/internal/` provides the services the core needs but cannot provide for itself:

- `runtime/` supplies real clocks, thread scheduling, and random number generation. In tests, these get replaced with deterministic fakes.
- `rpc/` wraps brpc to move Raft messages between nodes. The interface is written so that other transports (raw TCP, RDMA, in-memory loopback) can be plugged in without changing the core.
- `storage/` implements the log, metadata, and snapshot contracts against real backends (local segment files, RocksDB, SQLite). For tests, there are in-memory backends.
- `snapshot/` handles the mechanics of saving and loading snapshots: coordinating with the state machine, streaming data between nodes, and managing snapshot lifecycle.

## Why this is all internal

Keeping this code behind the `src/internal/` boundary means it can be refactored aggressively. Modules can be split, merged, or replaced. The cost is that users cannot depend on internal paths or types -- but that is the point. The public APIs in `include/quorumkit/` and `include/braft/` are the stable contracts. Everything behind them is free to move.
