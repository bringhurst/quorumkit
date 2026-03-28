---
title: Compatibility and migration
sidebar_position: 12
---

# Compatibility and migration

Compatibility is only useful if it gives people a path forward.

In QuorumKit, that path runs along two tracks. One is API compatibility through the braft surface. The other is storage compatibility through support for existing braft storage backends and migration into newer backend arrangements.

```mermaid
flowchart LR
    LegacyApi[braft API] --> CanonicalApi[QuorumKit API]
    LegacyStorage[braft Storage Backends] --> CanonicalStorage[QuorumKit Storage Contracts]
    CanonicalStorage --> Rocks[RocksDB Backend]
    CanonicalStorage --> Sqlite[SQLite Backend]
    CanonicalStorage --> Local[Local Segment Backend]
```
