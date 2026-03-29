---
title: Example programs
sidebar_position: 10
---

# Example programs

Three example programs live under `examples/`. Each one is a self-contained replicated service that you can build and run locally.

| Example | What it does | State machine |
|---|---|---|
| `counter` | Replicated integer counter | Increment only, single value |
| `atomic` | Replicated atomic register | Compare-and-swap on a single value |
| `block` | Block-oriented storage | Fixed-size block reads and writes |

`counter` is the simplest and the best place to start. `atomic` adds conditional writes. `block` exercises snapshot and log I/O more heavily.

Build the examples from the repository root with `-DBUILD_EXAMPLES=ON`, then use the per-example `run_server.sh` and `run_client.sh` scripts. The server scripts hard-code the peer addresses and ports -- read them to see how the initial configuration is passed.

For a step-by-step walkthrough, see [Build your first cluster](../tutorials/first-cluster).
