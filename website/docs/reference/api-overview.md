---
title: Public API overview
sidebar_position: 1
---

# Public API overview

QuorumKit has two sets of public headers. Both are installed with the library.

## `include/quorumkit/`

The primary API. New code should use these headers.

```text
include/quorumkit/
  types.h          shared types: peer IDs, group IDs, error codes
  node.h           creating and operating a Raft node
  admin.h          administrative operations: add/remove peer, transfer leader
  discovery.h      peer discovery and route table
  storage.h        storage backend contracts
  util.h           small utilities
  extensions/
    filesystem.h   filesystem snapshot adaptor
    throttle.h     snapshot throttle
```

`node.h` is where most users start. It defines the node lifecycle: create, start, apply, snapshot, shut down. The other headers support it. `storage.h` is where the backend contracts live, including the requirement that backend families support dual write and bootstrap from any other backend family. See [Storage backend contract](./storage-backend-contract).

## `include/braft/`

The compatibility API. Existing braft code can keep using these headers -- they forward into the QuorumKit implementation underneath.

```text
include/braft/
  raft.h                 core node, task, iterator, and state machine types
  node.h                 node declarations used by mock-based test code
  configuration.h        peer IDs and peer configuration
  snapshot.h             snapshot reader/writer surface used by applications
  storage.h              abstract snapshot interfaces used by tests
  util.h                 utilities such as AsyncClosureGuard
  errno.pb.h             Raft-specific error codes
```

Other braft headers may exist, but the precise compatibility promise lives in [braft compatibility contract](./braft-compatibility-contract).

## How the two relate

```mermaid
flowchart LR
    B[braft headers] --> Q[QuorumKit headers]
    Q --> I[Internal implementation]
```

The braft headers depend on the QuorumKit headers, not the other way around. QuorumKit defines the model; the braft layer adapts to it.

For more on the design behind each surface, see [Canonical public API](../explanation/canonical-public-api), [braft compatibility surface](../explanation/braft-compatibility-surface), and [braft compatibility contract](./braft-compatibility-contract).
