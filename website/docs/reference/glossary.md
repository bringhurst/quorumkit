---
title: Glossary
sidebar_position: 5
---

# Glossary

**Canonical API** -- The QuorumKit public headers under `include/quorumkit/`. New code should target this surface.

**Compatibility surface** -- The braft public headers under `include/braft/`. They preserve source compatibility for existing integrations and forward into the QuorumKit implementation.

**Deterministic core** -- The Raft state machine at the center of the library. It processes inputs in a fixed order and does not call the clock, the network, or the disk directly. This makes it possible to test under simulated time and simulated faults.

**Runtime adapter** -- Code that supplies the services the core cannot provide for itself: real clocks, thread scheduling, random number generation, and so on.

**Transport adapter** -- Code that moves Raft protocol messages between nodes. The core produces and consumes messages; the transport adapter decides whether they travel over brpc, raw TCP, RDMA, or an in-memory channel (for tests).

**Storage backend** -- A concrete implementation of log storage, metadata storage, or snapshot storage. Examples: local segment files, RocksDB, SQLite, in-memory (for tests).
