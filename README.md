# QuorumKit

QuorumKit is a C++ Raft library for consensus and replicated state machines.

It provides:

- a canonical `quorumkit` public API,
- a bounded `braft` compatibility surface,
- a core designed for formal reasoning,
- deterministic, FoundationDB-style testing around that core,
- modular transport, storage, and runtime adapters.

For details, start with:

- `docs/index.md`
- `docs/api/overview.md`
- `docs/architecture/index.md`

QuorumKit is distributed under the Apache License 2.0.

This repository is based on `braft`, and it preserves the original copyright and license notices where required.
