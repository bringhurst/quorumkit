---
title: Protocols And Features
sidebar_position: 11
---

# Protocols And Features

Not every part of the repository belongs in the same conceptual bucket.

Some pieces are the core of the replicated state machine: elections, replication, commit semantics, and application order. Other pieces are the machinery that makes a real system usable: snapshots, administration, discovery, witness support, leases, transport adapters, storage migration helpers, telemetry, and so on.

That distinction matters because QuorumKit is trying to keep the heart of the system small and understandable. The more the project can separate core behavior from surrounding features, the easier it becomes to reason about the core precisely while still supporting a practical library around it.

The compatibility layer complicates that picture a little, because some inherited features still need to exist for braft users. The goal is not to hide them. The goal is to place them in the right layer so they stop distorting the shape of the core.
