---
title: braft compatibility surface
sidebar_position: 4
---

# braft compatibility surface

The headers under `include/braft/` exist so that an application already built against the public braft API can keep compiling against QuorumKit.

The target use case is not a toy. It is an existing service that constructs `braft::Node`, subclasses `braft::StateMachine`, uses snapshots, manages peer changes, and carries a normal unit-test suite with braft mocks. QuorumKit wants that kind of codebase to treat the braft layer as a drop-in replacement.

That drop-in promise now also covers the practical compatibility details those services usually depend on: legacy `local://` storage URIs, the on-disk snapshot tree visible through `SnapshotReader::get_path()`, and the ordinary mockable snapshot/configuration helpers used in their tests.

The braft headers are thin adapters. Each one preserves the old names, namespaces, and calling patterns while forwarding into the QuorumKit implementation underneath. The engine is QuorumKit; the braft layer does not own a separate Raft core.

```mermaid
flowchart LR
    Legacy[Existing user code] --> B[include/braft headers]
    B --> BC[src/braft_compat adapters]
    BC --> Q[QuorumKit API + internals]
```

## What the compatibility layer preserves

At a high level, the compatibility layer preserves three things:

- the public header and namespace shape (`include/braft/`, `braft::`)
- the application-facing node/state-machine/snapshot surface that existing services build against
- the test-facing abstract interfaces that ordinary mock-based test suites use

The exact contract is intentionally written down in one place: [braft compatibility contract](../reference/braft-compatibility-contract).

## What it does not preserve

Internal implementation details, undocumented behavior, and log/storage inspection internals are not part of the stable promise. If code was reaching into braft internals rather than staying on the documented public surface, some adaptation may still be required.

The compatibility layer is supported and tested, but it is not where the API is growing. New features and new types show up in `include/quorumkit/` first. The braft layer gets what it needs to keep existing code working inside the documented contract.
