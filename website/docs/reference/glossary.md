---
title: Glossary
sidebar_position: 9
---

# Glossary

**Canonical API** -- The QuorumKit public headers under `include/quorumkit/`. New code should target this surface.

**Compatibility surface** -- The braft public headers under `include/braft/`. They preserve source compatibility for existing integrations and forward into the QuorumKit implementation.

**Deterministic core** -- The Raft state machine at the center of the library. It processes inputs in a fixed order and does not call the clock, the network, or the disk directly. This makes it possible to test under simulated time and simulated faults.

**Runtime adapter** -- Code that supplies the services the core cannot provide for itself: real clocks, thread scheduling, random number generation, and so on.

**Transport adapter** -- Code that moves Raft protocol messages between nodes. The core produces and consumes messages; the transport adapter decides whether they travel over brpc, raw TCP, RDMA, or an in-memory channel (for tests).

**Storage backend family** -- A coordinated implementation of log storage, metadata storage, and snapshot storage chosen together as one backend option.

**Dual write** -- A migration mode in which QuorumKit applies the same logical durable mutations to two backend families so either one can be used for cutover or rollback.

**Bootstrap** -- Initializing an empty destination backend family from the durable state exported by a source backend family.

**Canonical bootstrap image** -- A backend-neutral representation of durable Raft state used to bootstrap one backend family from another without requiring pairwise migration adapters.

**Wire compatibility** -- The requirement that QuorumKit continue speaking the legacy braft network protocol during mixed-version rollouts.

**Rolling migration** -- Replacing nodes in a live cluster without a flag day for application code, wire protocol, or durable state.

**Export cut** -- A logical point between durable mutations used to define one coherent storage snapshot for bootstrap and validation.

**Validation cut** -- A named logical point in the durable mutation history used when comparing source and destination state.

**Logical equivalence** -- Equality of Raft-visible durable state, without requiring identical on-disk bytes or layout.
