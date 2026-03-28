---
title: Storage layer
sidebar_position: 8
---

# Storage layer

Storage is one of the places where QuorumKit has to balance cleanup with compatibility.

The goal is a clean storage model: backend-neutral contracts, swappable implementations, deterministic fakes for tests, and enough separation from the core that changing persistence does not mean rewriting consensus logic. At the same time, the existing braft storage backends still matter, and migration between backends matters too.

```mermaid
flowchart TD
    Core[Consensus Core] --> Log[Log Storage]
    Core --> Meta[Metadata Storage]
    Core --> Snap[Snapshot Storage]
    Log --> Segment[Local Segment Backend]
    Log --> Memory[In-Memory Backend]
    Log --> Rocks[RocksDB Backend]
    Log --> Sqlite[SQLite Backend]
    Meta --> FileMeta[File or KV Meta Backend]
    Meta --> RocksMeta[RocksDB Meta Backend]
    Meta --> SqliteMeta[SQLite Meta Backend]
    Snap --> LocalSnap[Filesystem Snapshot Backend]
    Snap --> ObjSnap[Object Store Snapshot Backend]
```
