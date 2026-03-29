---
title: Build your first cluster
sidebar_position: 1
---

# Build your first cluster

You are going to build QuorumKit from source, start a three-node replicated counter on your local machine, and send it some requests. The whole thing takes a few minutes (plus time for the first Conan build).

## Install prerequisites

You need a C++17 compiler, CMake 3.15+, and Conan 2. See [Prerequisites](../how-to/prerequisites) for install commands.

## Build the library

From the repository root, export the local brpc recipe (one-time step):

```sh
conan export contrib/brpc/
```

Install all dependencies and generate CMake presets:

```sh
conan install . --output-folder=build --build=missing
```

Build:

```sh
cmake --preset conan-release
cmake --build --preset conan-release
```

When this finishes you should have `libbraft.a` and test binaries under `build/`. If something goes wrong, see [Troubleshoot common problems](../how-to/troubleshoot-common-problems).

## Run the tests

Confirm everything works:

```sh
ctest --preset conan-release --output-on-failure -LE known_crash
```

You should see 20 tests pass. The `-LE known_crash` flag skips three tests with upstream braft bugs.

## Build the counter example

The `counter` example is a small replicated counter service -- three servers that agree on a single integer.

```sh
cmake --preset conan-release -DBUILD_EXAMPLES=ON
cmake --build --preset conan-release
```

This produces the counter example binaries under `build/build/Release/examples/counter/`.

## Start the cluster

```sh
cd examples/counter
bash run_server.sh
```

This launches three server processes on localhost. Each one writes its Raft log and snapshot data into a subdirectory under `examples/counter/`. The script resolves `counter_server` from the CMake build tree, so you do not need to copy binaries around by hand. You will see log output showing leader election: one node wins an election and the other two become followers.

## Send requests

In a second terminal:

```sh
cd examples/counter
bash run_client.sh
```

The client sends increment requests to the cluster. You should see the counter value increasing in the client output. Behind the scenes, each request goes to the leader, gets replicated to a majority, and then gets applied to the counter state machine on all three nodes.

## What just happened

You ran the full Raft cycle: a client wrote to a leader, the leader replicated the write to followers, a majority acknowledged it, and all three nodes applied the same state transition in the same order. The counter example is intentionally minimal -- it exists so you can see that loop without any extra complexity.

## Where to go from here

- [Run the examples](../how-to/run-the-examples) covers the `atomic` and `block` examples
- [Public API overview](../reference/api-overview) describes the two public header surfaces
- [Architecture overview](../explanation/architecture-overview) explains how the library is put together
