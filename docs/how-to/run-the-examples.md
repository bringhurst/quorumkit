---
title: Run the examples
sidebar_position: 3
---

# Run the examples

All examples live under `example/`. Each one is a standalone CMake project that links against QuorumKit.

:::note
The examples are not yet wired into the Conan build system. They still use standalone CMake and link against the library directly. Building the main library first is required. This will be updated once the examples are integrated with Conan.
:::

## counter

A replicated integer counter. Three servers agree on a single number; the client increments it.

```sh
cd example/counter
cmake . && make
bash run_server.sh     # starts three local nodes
bash run_client.sh     # sends increment requests
```

The client prints the counter value after each successful request. Server logs show leader election and replication.

## atomic

A replicated compare-and-swap register. Same cluster setup, but the state machine supports atomic read-modify-write operations instead of a plain counter.

```sh
cd example/atomic
cmake . && make
bash run_server.sh
bash run_client.sh
```

## block

A block-oriented storage service with heavier I/O. The state machine reads and writes fixed-size blocks, so it exercises the snapshot and log paths more aggressively than the other two examples.

```sh
cd example/block
cmake . && make
bash run_server.sh
bash run_client.sh
```

## What to look at

Each example writes runtime state (Raft logs, snapshots, metadata) into subdirectories next to the server binary. After running one, poke around those directories to see what the library persists on disk.

The `run_server.sh` scripts hard-code a three-node cluster on localhost with different ports. Read the script to see how the initial configuration and peer list are passed.

For more detail on what each example demonstrates, see [Example programs](../reference/examples).
