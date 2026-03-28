---
title: Example programs
sidebar_position: 2
---

# Example programs

Three example programs live under `example/`. Each one is a self-contained replicated service that you can build and run locally.

| Example | What it does | State machine |
|---|---|---|
| `counter` | Replicated integer counter | Increment only, single value |
| `atomic` | Replicated atomic register | Compare-and-swap on a single value |
| `block` | Block-oriented storage | Fixed-size block reads and writes |

`counter` is the simplest and the best place to start. `atomic` adds conditional writes. `block` exercises snapshot and log I/O more heavily.

Each example has a `run_server.sh` that starts a three-node local cluster and a `run_client.sh` that sends requests. The server script hard-codes the peer addresses and ports -- read it to see how the initial configuration is passed.

For a step-by-step walkthrough, see [Build your first cluster](../tutorials/first-cluster).
