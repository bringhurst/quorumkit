---
title: Canonical public API
sidebar_position: 3
---

# Canonical public API

The QuorumKit API lives under `include/quorumkit`. That is the surface new applications should learn, document against, and build on.

The point is straightforward: the API should speak in terms that match the problem, not in terms inherited from old file layouts.

## The Header Set

```text
include/quorumkit/
  types.h
  node.h
  admin.h
  discovery.h
  storage.h
  util.h
  extensions/
    filesystem.h
    throttle.h
```

Each header has a clear job. `types.h` holds the shared nouns of the API. `node.h` is the center of the public surface. `admin.h`, `discovery.h`, and `storage.h` cover the operational side. `util.h` stays small, and `extensions/*` holds optional hooks.

## Why the surface looks this way

This API talks about nodes, groups, storage, and administration. It does not drag transport classes, thread-library types, protobuf service bases, or logging frameworks into every signature. A user should be able to understand the model without reverse-engineering brpc, bthread, or a storage plugin.
