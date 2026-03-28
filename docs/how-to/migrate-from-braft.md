---
title: Migrate from braft
sidebar_position: 4
---

# Migrate from braft

If you have existing code that uses braft, you do not need to change it all at once. The compatibility headers under `include/braft/` still work -- they call into the QuorumKit implementation underneath.

## Recommended order

1. **Keep running on the braft headers.** Your existing code compiles and works through the compatibility layer. Nothing breaks.

2. **Write new code against `include/quorumkit/`.** New modules, new services, new tests -- all of these should use the QuorumKit headers from the start.

3. **Migrate tests and examples.** Move your test code to the QuorumKit API. This is low-risk and gives you early experience with the new surface before touching production paths.

4. **Migrate application code.** Replace `#include <braft/...>` with `#include <quorumkit/...>` and update namespaces and type names. The braft compatibility layer can tell you where you still have old references -- if it compiles through `include/braft`, it hasn't been migrated yet.

5. **Plan storage migration separately.** If you have persisted Raft logs and snapshots on disk, changing the storage backend is a distinct step from changing headers. QuorumKit supports the existing braft storage backends, so you can migrate the API without touching storage. When you are ready to change backends, do it deliberately and with validation. See [Storage layer](../explanation/storage-layer).

## What the compatibility layer does and does not do

The braft headers are not frozen copies of old code. They are thin adapters that forward to the QuorumKit implementation. They preserve the old names, namespaces, and calling patterns. They do not preserve internal implementation details or undocumented behavior.

For the design rationale behind the compatibility boundary, see [Compatibility and migration](../explanation/compatibility-and-migration).
