---
title: Internal Architecture
sidebar_position: 5
---

# Internal Architecture

Everything under `src/internal` exists to support the public surfaces without becoming one.

That distinction matters. Once code is clearly internal, the project can split modules, replace implementations, or move pieces around without turning every refactor into a breaking API debate.

## The Main Internal Areas

```text
src/internal/
  core/
  runtime/
  rpc/
  storage/
  snapshot/
  proto/
  base/
```

`core/` holds the deterministic heart of the system. `runtime/` supplies clocks, schedulers, and execution adapters. `rpc/` owns message transport. `storage/` owns persistence. `snapshot/`, `proto/`, and `base/` round out the implementation.
