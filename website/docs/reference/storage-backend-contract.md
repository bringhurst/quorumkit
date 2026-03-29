---
title: storage backend contract
sidebar_position: 4
---

# storage backend contract

This page is the normative contract for storage backends in QuorumKit.

The rule is simple: a storage backend is not complete unless it can participate in migration. In practice that means every backend family must support:

- bootstrap from any other backend family
- dual write with any other backend family

This is a hard requirement, not a best-effort goal.

## Terms

- **Storage backend family** -- A coordinated implementation of log storage, metadata storage, and snapshot storage chosen together as one backend option. Examples: the local segment-file family, a RocksDB-backed family, an in-memory family for tests.
- **Bootstrap** -- Initializing an empty destination backend family from the durable state of a source backend family.
- **Dual write** -- A migration mode in which QuorumKit applies the same logical persistent mutations to two backend families: a primary backend and a mirror backend.
- **Canonical bootstrap image** -- A backend-neutral representation of durable Raft state. QuorumKit uses this to keep backend migration support linear in the number of backend families rather than requiring pairwise `A -> B` converters.

## Required capabilities

Every storage backend family MUST:

- export a canonical bootstrap image from its durable state
- import a canonical bootstrap image into an empty instance of that backend family
- act as the source side of bootstrap for any other backend family
- act as the destination side of bootstrap for any other backend family
- act as the primary side of a dual-write pair
- act as the mirror side of a dual-write pair

These requirements apply to the full persistent Raft state, not just log entries.

## Canonical bootstrap model

QuorumKit standardizes backend-to-backend bootstrap through a canonical bootstrap image.

This is deliberate. If there are `n` backend families, QuorumKit wants `O(n)` import/export implementations, not `O(n^2)` pairwise migration adapters.

So the compatibility rule is:

- backend family `A` satisfies "bootstrap to any other backend family" by exporting the canonical bootstrap image
- backend family `B` satisfies "bootstrap from any other backend family" by importing the canonical bootstrap image

Backend-specific direct converters may exist, but they are not the compatibility contract. The required common representation is defined in [canonical bootstrap image](./canonical-bootstrap-image).

## Canonical bootstrap contents

The canonical bootstrap image MUST contain enough information to reconstruct the source backend's durable Raft state at a well-defined export cut.

At minimum that means:

- the retained log range
- for every retained log entry in that range, the entry index, term, type, and payload as observed through the storage contract
- persisted Raft metadata needed for safe restart, including current term and voted-for peer
- the latest durable snapshot metadata, including last included index and term
- the latest durable snapshot contents as observed through the snapshot reader contract

If a backend family persists additional Raft-critical state beyond these items, that state MUST also round-trip through the canonical bootstrap image.

## Bootstrap requirements

For any source backend family `A` and any destination backend family `B`:

- `B` MUST be able to bootstrap from a canonical bootstrap image exported by `A`
- bootstrap into `B` MUST require an empty destination instance
- the import result MUST be deterministic for a given canonical bootstrap image
- a node that can restart safely on the exported state from `A` MUST also be able to restart safely on the imported state in `B`

After bootstrap, `B` MUST be equivalent to `A` at the export cut.

In this contract, equivalent means:

- `A` and `B` expose the same retained log index range
- for every retained log index, `A` and `B` expose the same logical log entry contents
- `A` and `B` expose the same persisted term and voted-for information
- `A` and `B` expose the same latest snapshot metadata
- `A` and `B` expose the same logical snapshot contents through the snapshot reader contract

Equivalent does not require identical on-disk layout. Different backend families are free to serialize and organize bytes differently as long as the Raft-visible state is the same. The comparison rules are defined in [state equivalence and validation](./state-equivalence-and-validation).

## Dual-write requirements

Dual write is the mechanism that keeps two backend families aligned after bootstrap and before cutover.

For any ordered backend pair `(A, B)`:

- `A` MUST be usable as the primary backend while `B` acts as the mirror backend
- `B` MUST be usable as the primary backend while `A` acts as the mirror backend

Dual write MUST cover every logical persistent mutation required to restart Raft safely, including:

- log appends
- log truncation and reset operations
- metadata updates
- snapshot publish/update operations
- any backend-visible cleanup or replacement step that changes the durable state seen after restart

A dual-written mutation MUST not be reported as durably complete until both the primary backend and the mirror backend have accepted it, or the operation has failed.

Dual write is defined only for related histories:

- the mirror backend MUST start from an empty instance
- the mirror backend MUST first be bootstrapped from the primary backend
- after that point, the mirror backend MUST stay current through dual-written mutations alone

Once a backend family has been bootstrapped and kept current by dual write, it MUST be possible to cut over to it without running a second one-off migration step.

## Cutover and rollback

The storage contract is designed to support a practical migration sequence:

1. export from source backend family `A`
2. bootstrap destination backend family `B`
3. run `A` as primary and `B` as mirror in dual-write mode
4. validate equivalence
5. cut over so `B` becomes the active backend family

During step 3, `A` remains a rollback target because it continues to receive the same durable mutations. That rollback property is one of the reasons dual write is mandatory.

## What this contract does not require

This contract does not require:

- identical on-disk file layout across backend families
- identical internal checksumming or compaction strategies
- pairwise custom migration code for every backend combination

It does require equivalent Raft-visible durable state and a repeatable migration path.

For how this storage contract fits into a full cluster rollout, see [rolling migration contract](./rolling-migration-contract).
