---
title: Testing architecture
sidebar_position: 9
---

# Testing architecture

QuorumKit is set up so the testing story follows the architecture instead of fighting it.

```text
test/
  public/
    quorumkit/
    braft_compat/
  internal/
    core/
    runtime/
    transport/
    storage/
    snapshot/
  simulation/
    cluster/
    fault/
    workload/
```

Public behavior gets tested through public APIs. Internal mechanisms get tested in smaller pieces. Cluster behavior gets tested under a deterministic simulation environment.
