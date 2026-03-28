---
title: Public API overview
sidebar_position: 1
---

# Public API overview

QuorumKit has two public surfaces.

New code should use `include/quorumkit`. Existing code can keep using `include/braft`. Both are supported. `quorumkit` is the main API; `braft` is the compatibility layer.

## The QuorumKit surface

The QuorumKit headers are organized by concept rather than by implementation history.

```text
include/quorumkit/
  types.h
  node.h
  admin.h
  discovery.h
  storage.h
  util.h
  extensions/
    filesystem.h
    throttle.h
```

Together, these headers describe the system in the terms most users care about: identities, nodes, administration, discovery, persistence, and a few optional extensions.

## The braft surface

The compatibility headers keep the old vocabulary available.

```text
include/braft/
  configuration.h
  raft.h
  cli.h
  route_table.h
  storage.h
  util.h
  file_system_adaptor.h
  snapshot_throttle.h
  protobuf_file.h
```

That surface is there so existing integrations keep compiling while the rest of the project moves in a cleaner direction.

## How the two surfaces relate

```mermaid
flowchart LR
    B[braft Compatibility API] --> Q[QuorumKit API]
    Q --> I[Internal Implementation]
```

That direction matters. QuorumKit defines the model. The braft layer adapts to it. Old names are not a reason to grow a second API design.

## Where to go next

- For the public API philosophy, read [Canonical public API](../explanation/canonical-public-api).
- For compatibility details, read [braft compatibility surface](../explanation/braft-compatibility-surface).
