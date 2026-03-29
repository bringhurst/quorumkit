---
title: Run the examples
sidebar_position: 3
---

# Run the examples

All examples live under `examples/`. They build through the main QuorumKit CMake project, so they reuse the same Conan-managed dependencies and the same `braft` target as the library.

Before running any example, build the repo from the root with examples enabled:

```sh
conan export contrib/brpc/
conan install . --output-folder=build --build=missing
cmake --preset conan-release -DBUILD_EXAMPLES=ON
cmake --build --preset conan-release
```

The run scripts look for binaries in the source directory first, then in `build/build/Release/examples/` and `build/build/Debug/examples/`.

## counter

A replicated integer counter. Three servers agree on a single number; the client increments it.

```sh
cd examples/counter
bash run_server.sh     # starts three local nodes
bash run_client.sh     # sends increment requests
```

The client prints the counter value after each successful request. Server logs show leader election and replication.

## atomic

A replicated compare-and-swap register. Same cluster setup, but the state machine supports atomic read-modify-write operations instead of a plain counter.

```sh
cd examples/atomic
bash run_server.sh
bash run_client.sh
```

## block

A block-oriented storage service with heavier I/O. The state machine reads and writes fixed-size blocks, so it exercises the snapshot and log paths more aggressively than the other two examples.

```sh
cd examples/block
bash run_server.sh
bash run_client.sh
```

## What to look at

Each example writes runtime state (Raft logs, snapshots, metadata) into subdirectories next to the server binary. After running one, poke around those directories to see what the library persists on disk.

The `run_server.sh` scripts hard-code a three-node cluster on localhost with different ports. Read the script to see how the initial configuration and peer list are passed.

For more detail on what each example demonstrates, see [Example programs](../reference/examples).
