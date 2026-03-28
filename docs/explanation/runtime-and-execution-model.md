---
title: Runtime and execution model
sidebar_position: 6
---

# Runtime and execution model

QuorumKit treats the core as a state machine. The runtime is everything around it that makes it useful in a real program.

```mermaid
flowchart LR
    App[User Application] --> API[QuorumKit Public API]
    API --> Core[Deterministic Core]
    Core --> Clock[Clock]
    Core --> Rand[Random Source]
    Core --> Sched[Scheduler]
    Core --> Net[Transport]
    Core --> Store[Storage]
```

The core should not decide what time it is, how work is scheduled, how packets move, or how bytes reach disk. It asks for those services through interfaces.
