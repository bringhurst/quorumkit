---
title: Runtime and execution model
sidebar_position: 6
---

# Runtime and execution model

QuorumKit treats the core of the system as a state machine and the runtime as everything needed to let that state machine live in the world.

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

The core should not decide what time it is, how work gets scheduled, how packets move, or how bytes reach disk. It asks for those services through interfaces.
