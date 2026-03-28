---
title: braft compatibility surface
sidebar_position: 4
---

# braft compatibility surface

The braft layer is public, supported, and intentionally secondary.

Real code already depends on braft headers, type names, and calling patterns. QuorumKit keeps that code working, but it does so through a clearly named compatibility layer rather than pretending braft is still the center of the project.

## How The Layer Sits In The System

```mermaid
flowchart LR
    Legacy[Existing User Code] --> B[braft Headers]
    B --> BC[braft Compatibility Adapters]
    BC --> Q[QuorumKit Public API]
    Q --> I[Internal Implementation]
```

The compatibility layer points inward. It does not own the engine, define the runtime, or become a second place where the real design lives.

## What It Preserves

It keeps the old names, old namespaces, old route-table style helpers, old administration entry points, and the existing storage construction patterns that matter for compatibility. It also keeps the existing braft storage family in play so old deployments still have a path forward.
