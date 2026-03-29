---
title: rolling migration contract
sidebar_position: 7
---

# rolling migration contract

This page defines what QuorumKit means by a supported rolling migration.

A rolling migration is supported only if QuorumKit can replace nodes in a live deployment without requiring a flag day for application code, network protocol, or durable state.

That means three lower-level contracts all have to hold at the same time:

- [braft compatibility contract](./braft-compatibility-contract)
- [wire compatibility contract](./wire-compatibility-contract)
- [storage backend contract](./storage-backend-contract)

## Supported migration shapes

This contract covers these migration shapes:

- binary replacement while keeping the same storage backend family
- storage-backend migration while keeping the same application-facing API surface
- staged migration where binary replacement and storage migration happen together, as long as each stage still satisfies the contracts above

## Preconditions

Before calling a migration supported, QuorumKit assumes:

- the application either stays on the documented `include/braft/` contract or already targets `include/quorumkit/`
- mixed-version nodes still speak the legacy braft wire dialect
- the source and destination storage backend families satisfy the storage contract
- the operator has a way to validate durable-state equivalence before cutover

For existing braft deployments, this includes the common case where the node keeps using legacy `local://` storage URIs while the binary changes underneath it.

## Node-level guarantees

During a supported rolling migration, a migrated node MUST be able to:

- restart from its configured durable state safely
- rejoin a cluster containing non-migrated peers
- exchange Raft traffic with legacy peers through the legacy wire contract
- participate in elections, log replication, snapshot installation, and leadership transfer

If the node is migrating storage backend families as part of the rollout, it MUST also satisfy the storage contract for bootstrap, dual write, validation, cutover, and rollback.

## Cluster-level guarantees

During a supported rolling migration, the cluster as a whole MUST remain able to:

- keep quorum using mixed-version nodes
- elect leaders across mixed-version peers
- replicate logs across mixed-version peers
- install snapshots across mixed-version peers
- perform administrative operations through the legacy admin surface

The storage backend family used by a node MUST NOT alter the node's wire behavior during the mixed-version phase.

## Storage-migration sequence

When a node changes storage backend families, the required contract sequence is:

1. export durable state from source backend family `A`
2. bootstrap destination backend family `B`
3. validate `B` against `A` at the export cut
4. run `A` and `B` in dual-write mode
5. validate `B` against `A` at a cutover validation cut
6. cut over so `B` becomes authoritative

Until step 6 is accepted, `A` remains the rollback target.

## Success conditions

A rolling migration is successful only when:

- every migrated node satisfies the API, wire, and storage contracts relevant to its stage
- the cluster remains healthy throughout the mixed-version period
- the final post-migration cluster can restart safely from the durable state left behind on each node

## Unsupported migration styles

This contract does not cover:

- changing to a new wire dialect in the middle of a mixed-version rollout without explicit separate protocol negotiation
- one-shot blind storage conversion without bootstrap, validation, and dual write
- depending on undocumented braft internals instead of the documented compatibility surface

Those migrations may be possible in theory. They are just not part of the contract QuorumKit intends to support and eventually test.
