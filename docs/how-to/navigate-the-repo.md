---
title: Navigate the repository
sidebar_position: 2
---

# Navigate the repository

When you first open QuorumKit, the most useful question is not “where is the Raft code?” It is “what here is public, and what here is internal?”

Start with these roots:

```text
include/
src/
test/
docs/
example/
```

Then use the internal split:

- `include/quorumkit` is the canonical public API.
- `include/braft` is the compatibility API.
- `src/internal` is implementation detail.
- `src/braft_compat` is the adapter layer for braft compatibility.

If you want the longer explanation of why the tree is organized that way, read [Repository Layout](../explanation/repository-layout).
