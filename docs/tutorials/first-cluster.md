---
title: Build your first cluster
sidebar_position: 1
---

# Build your first cluster

This tutorial is the shortest path to getting your bearings.

The goal is not to teach every part of QuorumKit at once. The goal is to get the project building, run one of the sample applications, and connect the moving pieces to the words you will see elsewhere in the docs.

By the end, you will know where the examples live, how the local build works, what a cluster looks like in this repository, and which docs to read next.

## Step 1: build the repo

From the repository root:

```sh
cmake -S . -B bld
cmake --build bld
```

If you want the test binaries as well, add:

```sh
cmake -S . -B bld -DBUILD_UNIT_TESTS=ON
cmake --build bld
```

This gives you a local build tree and verifies that the project is in a usable state.

## Step 2: pick an example

The examples live under `example/`.

- `counter` is the smallest place to start.
- `atomic` shows a slightly richer replicated state model.
- `block` is more operationally involved.

For a first pass, use `counter`.

## Step 3: build and run the example

```sh
cd example/counter
cmake .
make
bash run_server.sh
```

This starts a small local cluster and writes runtime state under the example directory.

In a second shell, run the client:

```sh
bash run_client.sh
```

At this point you are looking at the full loop: client request, leader handling, replication, commit, and state machine application.

## Step 4: connect what you saw to the docs

Once the example is running, the rest of the docs become easier to read.

- For the big picture, read [Architecture overview](../architecture) and [Canonical public API](../explanation/canonical-public-api).
- To understand the public surfaces, read [Public API overview](../reference/api-overview).
- To understand the internal split, read [Repository layout](../explanation/repository-layout).

## What to read next

If you want to keep exploring, the usual order is:

1. [Navigate the repository](../how-to/navigate-the-repo)
2. [Run the examples](../how-to/run-the-examples)
3. [Public API overview](../reference/api-overview)
4. [Architecture overview](../architecture)

This tutorial is intentionally narrow. It gets you from zero to a running cluster and then hands off to the rest of the documentation.
