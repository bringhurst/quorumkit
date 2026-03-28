---
title: Public API overview
sidebar_position: 1
---

# Public API overview

QuorumKit exposes two public faces.

The first is the one new code should use: `include/quorumkit`. It is the vocabulary the project wants to stand behind long term. The second is `include/braft`, a compatibility surface for code that already speaks in braft terms. Both are public. Only one is canonical.

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

Taken together, these headers describe the system the way a user thinks about it: identities, nodes, administration, discovery, persistence, and a small set of optional extensions.

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

That surface exists so existing integrations can keep compiling while the repository moves toward a cleaner internal shape.

## How the two surfaces relate

```mermaid
flowchart LR
    B[braft Compatibility API] --> Q[QuorumKit API]
    Q --> I[Internal Implementation]
```

That direction matters. QuorumKit defines the model. The braft layer adapts to it. The project does not grow a second independent public model just because old names still exist.

## Where to go next

- For the public API philosophy, read [Canonical public API](../explanation/canonical-public-api).
- For compatibility details, read [braft compatibility surface](../explanation/braft-compatibility-surface).
