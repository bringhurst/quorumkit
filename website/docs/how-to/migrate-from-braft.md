---
title: Migrate from braft
sidebar_position: 4
---

# Migrate from braft

If you have existing code that uses braft, you do not need to change it all at once. The compatibility headers under `include/braft/` still work -- they call into the QuorumKit implementation underneath.

The first question is not "how fast should I migrate?" It is "does my code stay inside the documented compatibility contract?" If it does, QuorumKit intends it to compile unchanged through the braft layer. See [braft compatibility contract](../reference/braft-compatibility-contract).

If you are planning a live rollout rather than an offline replacement, the broader design is described in [rolling migration contract](../reference/rolling-migration-contract). That is the page that ties source compatibility, wire compatibility, and storage migration together.

## Recommended order

1. **Keep running on the braft headers.** If your code only uses the documented compatibility surface, it should keep compiling and working through the compatibility layer.

2. **Write new code against `include/quorumkit/`.** New modules, new services, new tests -- all of these should use the QuorumKit headers from the start.

3. **Migrate tests and examples.** Move your test code to the QuorumKit API. This is low-risk and gives you early experience with the new surface before touching production paths.

4. **Migrate application code.** Replace `#include <braft/...>` with `#include <quorumkit/...>` and update namespaces and type names. The braft compatibility layer can tell you where you still have old references -- if it still compiles only through `include/braft`, it has not been migrated yet.

5. **Plan storage migration separately.** If you have persisted Raft logs and snapshots on disk, changing the storage backend is a distinct step from changing headers. QuorumKit supports the existing braft storage backends, so you can migrate the API without touching storage. When you are ready to change backends, use the storage migration path QuorumKit requires: bootstrap the target backend family from the current one, run both in dual-write mode, validate equivalence, then cut over. See [Storage layer](../explanation/storage-layer) and [Storage backend contract](../reference/storage-backend-contract).

## What the compatibility layer does and does not do

The braft headers are not frozen copies of old code. They are thin adapters that forward to the QuorumKit implementation. They preserve the old names, namespaces, and calling patterns listed in the compatibility contract. They do not preserve internal implementation details, log-storage internals, or undocumented behavior.

For the design rationale behind the compatibility boundary, see [Compatibility and migration](../explanation/compatibility-and-migration).
