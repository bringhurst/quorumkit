---
title: Canonical Public API
sidebar_position: 3
---

# Canonical Public API

The QuorumKit API lives under `include/quorumkit`. That is the surface new applications should learn, document against, and build on.

The point of this layout is not just cosmetic. It gives the library a vocabulary that matches the problem domain instead of mirroring whatever happened to exist in the implementation years ago.

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

Each header has a clear job. `types.h` holds the shared nouns of the API. `node.h` is the heart of the public surface. `admin.h`, `discovery.h`, and `storage.h` round out the operational model. `util.h` stays small, and `extensions/*` holds optional hooks.

## Why The Surface Looks This Way

The QuorumKit API is trying to be stable in the right places and quiet in the wrong ones.

It talks about nodes, groups, storage, and administration. It does not drag transport classes, thread-library types, protobuf service bases, or logging frameworks into every signature. A user should be able to understand the public model of the system without reverse-engineering brpc, bthread, or a specific storage plugin.
