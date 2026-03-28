---
title: Build your first cluster
sidebar_position: 1
---

# Build your first cluster

You are going to build QuorumKit from source, start a three-node replicated counter on your local machine, and send it some requests. The whole thing takes a few minutes.

## Build the repo

From the repository root:

```sh
cmake -S . -B bld
cmake --build bld
```

When this finishes you should have build artifacts under `bld/`. If CMake cannot find your compiler or complains about missing dependencies, see [Prerequisites](../how-to/prerequisites).

To also build the test binaries:

```sh
cmake -S . -B bld -DBUILD_UNIT_TESTS=ON
cmake --build bld
```

## Build the counter example

The `counter` example is a small replicated counter service -- three servers that agree on a single integer.

```sh
cd example/counter
cmake .
make
```

This produces the server and client binaries in the `example/counter` directory.

## Start the cluster

```sh
bash run_server.sh
```

This launches three server processes on localhost. Each one writes its Raft log and snapshot data into a subdirectory under `example/counter/`. You will see log output showing leader election: one node wins an election and the other two become followers.

## Send requests

In a second terminal:

```sh
cd example/counter
bash run_client.sh
```

The client sends increment requests to the cluster. You should see the counter value increasing in the client output. Behind the scenes, each request goes to the leader, gets replicated to a majority, and then gets applied to the counter state machine on all three nodes.

## What just happened

You ran the full Raft cycle: a client wrote to a leader, the leader replicated the write to followers, a majority acknowledged it, and all three nodes applied the same state transition in the same order. The counter example is intentionally minimal -- it exists so you can see that loop without any extra complexity.

## Where to go from here

- [Run the examples](../how-to/run-the-examples) covers the `atomic` and `block` examples
- [Public API overview](../reference/api-overview) describes the two header surfaces
- [Architecture overview](../explanation/architecture-overview) explains how the library is put together
