---
title: Examples
sidebar_position: 3
---

# Examples

The example programs are small replicated applications that demonstrate hosting a state machine on top of the library.

## Example Directory

```text
example/
  atomic/
  block/
  counter/
```

## What Each Example Shows

### `counter`

The counter example shows a minimal replicated service with request forwarding, command application, and snapshot save and load behavior.

### `atomic`

The atomic example shows a small replicated state model with compare-and-exchange style behavior and a compact client interaction loop.

### `block`

The block example shows a more IO-heavy replicated service with random access behavior and snapshot-aware persistence.

## How To Read The Examples

Read the examples after understanding the public surfaces.

The recommended reading order is:

1. `docs/api/overview.md`
2. `docs/architecture/runtime.md`
3. one example server implementation
4. the matching client implementation

This order makes the example code easier to interpret because the roles of node lifecycle, state machine callbacks, storage, transport, and client redirection are already defined.

## Build And Run

Each example contains its own local build instructions in `example/README.md`.

The examples are intended as runnable guides, not as the source of truth for API contracts. The source of truth remains the public headers and the docs tree.
