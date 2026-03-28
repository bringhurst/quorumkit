---
title: braft Compatibility Surface
sidebar_position: 4
---

# braft Compatibility Surface

The braft layer is public, supported, and intentionally secondary.

It exists for a simple reason: real code already depends on braft headers, type names, and calling patterns. QuorumKit keeps that code working, but it does so by treating compatibility as a clearly named layer rather than letting it keep masquerading as the center of the project.

## What Lives In The Compatibility Surface

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

These headers remain installed because existing users need them. The important shift is conceptual: they are preserved as compatibility, not as the language new code is expected to grow around.

## What Compatibility Means Here

QuorumKit aims for source compatibility, not binary identity. Existing code should still compile against braft names and APIs. That does not mean the project freezes its internal layout or promises that the old object model remains untouched forever.

## How The Layer Sits In The System

```mermaid
flowchart LR
    Legacy[Existing User Code] --> B[braft Headers]
    B --> BC[braft Compatibility Adapters]
    BC --> Q[QuorumKit Public API]
    Q --> I[Internal Implementation]
```

The compatibility layer points inward. It does not own the engine, define the runtime, or become a second place where the real design lives.

## What It Preserves

It keeps the old names, old namespaces, old route-table style helpers, old administration entry points, and the existing storage construction patterns that matter for compatibility. It also keeps the existing braft storage family in play so old deployments still have a path forward.

Some older assumptions stay here on purpose: brpc-oriented service registration, braft-style address parsing, URI conventions inherited from the original system, and protobuf-shaped helper behavior where the old surface exposed them.

## What It Should Not Become

`src/braft_compat` should stay thin. Adapters, conversions, forwarding wrappers, and compatibility tests belong there. Consensus logic, replication mechanics, snapshot coordination, and fresh feature design do not.

If too much real implementation starts piling up in the compatibility layer, the project has recreated the problem it was trying to solve.
