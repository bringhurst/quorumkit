---
title: braft compatibility surface
sidebar_position: 4
---

# braft compatibility surface

The headers under `include/braft/` exist so that code already written against braft keeps compiling.

They are thin adapters. Each braft header includes the corresponding QuorumKit header and maps the old names, namespaces, and calling patterns onto the new API. The implementation underneath is QuorumKit -- the braft layer does not have its own copy of the engine.

```mermaid
flowchart LR
    Legacy[Existing user code] --> B[include/braft headers]
    B --> BC[src/braft_compat adapters]
    BC --> Q[QuorumKit API + internals]
```

## What the compatibility layer preserves

- The braft header names (`raft.h`, `configuration.h`, `cli.h`, etc.)
- The `braft::` namespace and type names
- The route-table and admin calling patterns
- The existing storage construction patterns (so old deployments still work)

## What it does not preserve

Internal implementation details, undocumented behavior, and direct access to braft internals. If your code was reaching into braft source files rather than public headers, it may need changes.

The compatibility layer is supported and tested, but it is not where the API is growing. New features and new types show up in `include/quorumkit/` first. The braft layer gets whatever it needs to keep existing code working.
