---
title: Canonical public API
sidebar_position: 3
---

# Canonical public API

The QuorumKit API lives under `include/quorumkit/`. It is the surface new applications should build against.

## The headers

```text
include/quorumkit/
  types.h           shared types: PeerId, GroupId, error codes
  node.h            creating and operating a Raft node
  admin.h           add/remove peer, transfer leader, reset peers
  discovery.h       peer discovery and route table
  storage.h         storage backend contracts
  util.h            small utilities
  extensions/
    filesystem.h    filesystem snapshot adaptor
    throttle.h      snapshot throttle
```

`node.h` is the center. It defines the node lifecycle: construct a node, configure its peers, start it, apply operations through it, snapshot it, shut it down. The other headers support that lifecycle -- `types.h` gives the shared vocabulary, `admin.h` and `discovery.h` cover cluster management, and `storage.h` defines the contracts that storage backends implement.

## What the API avoids

The old braft headers pull in brpc types, bthread types, protobuf service bases, and storage implementation details. The QuorumKit headers do not. You can read `node.h` and understand the node model without knowing anything about brpc or bthread.

This is a deliberate choice. Transport, threading, and storage are internal concerns. The public API talks about nodes, peers, groups, and operations -- the concepts you actually think about when you are writing a replicated service.

The `extensions/` directory holds optional pieces (filesystem adaptors, throttle hooks) that not every user needs. They are public but separate, so they do not clutter the core headers.
