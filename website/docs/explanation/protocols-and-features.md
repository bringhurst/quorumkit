---
title: Protocols and features
sidebar_position: 11
---

# Protocols and features

Not everything in QuorumKit is Raft consensus. The library has a small core protocol and a larger set of features built around it.

## The core protocol

The core handles the things defined by the Raft paper: leader election, log replication, commitment, and log application ordering. This code lives in `src/internal/core/` and is written to be deterministic -- it does not call the clock, the network, or the disk directly. It is the part of the system that you should be able to reason about on paper and test under simulation.

## Features around the core

Everything else is a feature that makes the core useful in production:

- **Snapshots** -- compressing the state machine to a checkpoint so the log does not grow without bound. Includes save, load, and streaming between nodes.
- **Administration** -- adding and removing peers, transferring leadership, resetting the peer configuration.
- **Discovery** -- finding peers and maintaining a route table.
- **Witness support** -- non-voting members that receive the log but do not participate in elections or quorum.
- **Leases** -- read leases that avoid the overhead of a full Raft read on every query.
- **Wire compatibility** -- keeping the legacy braft network protocol stable during mixed-version rollouts.
- **Storage migration** -- bootstrap, dual write, validation, cutover, and rollback for moving between backend families.
- **Telemetry** -- hooks for observing internal state and performance.
- **Transport adapters** -- plugging in different network transports.

These features depend on the core but the core does not depend on them. You can understand the core without knowing about snapshots, and you can add a new transport adapter without modifying election logic.
