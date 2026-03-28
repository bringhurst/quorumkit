---
title: Architecture overview
sidebar_position: 1
slug: /architecture
---

# Architecture overview

QuorumKit has two public APIs and one implementation beneath them.

- `quorumkit` is the API the project is built around.
- `braft` remains as a compatibility layer.
- `internal` is where the engine, adapters, and test infrastructure live.

That sounds simple, but it changes the shape of the repository in an important way. Public code becomes easy to spot. Compatibility becomes explicit. Internal code gets room to change aggressively without dragging users through every refactor.

## The shape of the system

```mermaid
flowchart TD
    Q[QuorumKit Public API\ninclude/quorumkit] --> I[Internal QuorumKit Implementation\nsrc/internal]
    B[braft Compatibility API\ninclude/braft] --> Q
    I --> R[Runtime Adapters\nthreading, clocks, scheduling]
    I --> T[Transport Adapters\nbrpc and others]
    I --> S[Storage Adapters\nlocal, memory, rocksdb, sqlite]
    Q --> P[Public API Tests]
    B --> C[Compatibility Tests]
    I --> D[Deterministic and Integration Tests]
```

The key idea is that the public surface stays small while the implementation grows inward, not outward.

## What this architecture optimizes for

The project is trying to make a few things true at the same time.

First, new code should read like QuorumKit, not like a historical fork. Second, compatibility should be honest about what it is: a bridge, not a second center of gravity. Third, the consensus core should stay small enough to reason about clearly, while the surrounding system remains modular enough to swap transports, storage engines, runtimes, and packaging strategies without rewriting the heart of the library.

The testing story follows the same logic. Public behavior should be easy to test directly. The core should be able to run under deterministic clocks, transports, and storage fakes. The whole system should be open to simulation rather than only end-to-end smoke tests.

## Read the rest of the explanation section

- [Repository layout](/docs/explanation/repository-layout)
- [Canonical public API](/docs/explanation/canonical-public-api)
- [braft compatibility surface](/docs/explanation/braft-compatibility-surface)
- [Internal architecture](/docs/explanation/internal-architecture)
- [Runtime and execution model](/docs/explanation/runtime-and-execution-model)
- [Transport layer](/docs/explanation/transport-layer)
- [Storage layer](/docs/explanation/storage-layer)
- [Testing architecture](/docs/explanation/testing-architecture)
- [Build and packaging modularity](/docs/explanation/build-and-packaging-modularity)
- [Protocols and features](/docs/explanation/protocols-and-features)
- [Compatibility and migration](/docs/explanation/compatibility-and-migration)
