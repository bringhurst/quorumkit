---
title: Protocols And Features
sidebar_position: 11
---

# Protocols And Features

QuorumKit is organized around a replicated state machine core and a set of protocol and feature modules layered around it.

## Core Versus Features

The architecture distinguishes between:

- the minimal replicated state machine core,
- protocol mechanisms required to operate that core,
- optional or deployment-specific feature modules.

This distinction matters because the repository is designed for both practical deployment and formal reasoning about a smaller core.

## Core Responsibilities

The core is responsible for:

- ordered command replication,
- leader election and term progression,
- committed-log semantics,
- membership-aware command flow,
- state machine application ordering.

## Surrounding Feature Modules

Surrounding modules include:

- snapshotting,
- administrative control operations,
- discovery helpers,
- witness support,
- readonly behavior,
- leader lease behavior,
- storage migration helpers,
- transport adapters,
- telemetry adapters.

## Why The Split Matters

Feature separation keeps the canonical API understandable and keeps the internal core small enough to reason about precisely.

The repository does not treat every inherited feature as equally foundational. Instead, each feature has an explicit place in the architecture.

## Relationship To Compatibility

Some features exist in the repository because the braft compatibility surface preserves them. The canonical QuorumKit documentation still describes them, but it places them within the correct architectural layer instead of presenting them as inseparable from the core.
