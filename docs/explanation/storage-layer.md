---
title: Storage Layer
sidebar_position: 8
---

# Storage Layer

Storage is one of the places where QuorumKit has to be both ambitious and practical.

The library wants a clean storage model: backend-neutral contracts, swappable implementations, deterministic fakes for tests, and enough separation from the core that changing persistence does not mean rewriting consensus logic. At the same time, it has to respect the world it comes from. Existing braft storage backends matter, and migration between backends matters just as much.

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
