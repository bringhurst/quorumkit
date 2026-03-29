---
title: state equivalence and validation
sidebar_position: 6
---

# state equivalence and validation

This page defines how QuorumKit decides whether two storage states are equivalent enough for bootstrap, dual write, cutover, and rollback.

The key rule is that QuorumKit validates logical durable Raft state, not raw byte identity.

## Terms

- **Validation cut** -- A named logical point in the durable mutation history used for comparison.
- **Source state** -- The backend family currently treated as authoritative at a given validation cut.
- **Destination state** -- The backend family being validated against the source state.

## Logical equivalence

At a given validation cut, two durable states are logically equivalent if all of the following hold:

- they expose the same retained log index range
- for every retained log index, they expose the same logical log entry contents
- they expose the same persisted term and voted-for information
- they expose the same latest durable snapshot metadata
- they expose the same logical snapshot contents through the snapshot reader contract, including the same relative file tree rooted at `SnapshotReader::get_path()`

Logical equivalence does not require the same bytes on disk, the same file layout, or the same compaction strategy.

## Entry equality

Two retained log entries are equal for this contract if they match on all Raft-visible contents needed to replay the log correctly, including:

- index
- term
- logical entry type
- logical payload

Backend-private framing, checksum placement, compression layout, and segment boundaries are not part of entry equality.

## Snapshot equality

Two latest durable snapshots are equal for this contract if they match on:

- last included index
- last included term
- the logical contents exposed through the snapshot reader contract
- the relative file paths and file bytes made visible under `SnapshotReader::get_path()`

They do not need to match on backend-private storage structure outside the public snapshot tree. The public relative layout visible from `SnapshotReader::get_path()` is part of the contract.

## Validation stages

QuorumKit uses the same logical equivalence model at several stages:

### After bootstrap

After bootstrapping backend family `B` from backend family `A`, `B` MUST be logically equivalent to `A` at the export cut used to create the bootstrap image.

### During dual write

During dual write, the mirror backend family MUST remain logically equivalent to the primary backend family at every completed validation cut after the mirror was bootstrapped.

### Before cutover

Before cutover, the destination backend family MUST be shown equivalent to the source backend family at a chosen validation cut that is recent enough for the cutover policy.

### After cutover

After cutover, the new primary backend family MUST be restart-safe on its own and MUST still expose the durable state validated at the cutover cut.

## Acceptable proof of equivalence

A final cutover decision MUST be based on a strong proof of equivalence.

Examples of acceptable proof include:

- a full logical comparison of the relevant durable state
- matching deterministic digests computed over the same canonical representation

Sampling MAY be used as an early warning or monitoring signal, but sampling alone MUST NOT be the only final cutover proof.

## Validation outputs

Validation logic SHOULD produce a report that makes mismatches actionable. At minimum, a mismatch report should identify:

- whether the mismatch is in logs, metadata, or snapshots
- the earliest mismatching log index, if logs differ
- whether the mismatch is structural, content-related, or freshness-related

## Relationship to other contracts

- [storage backend contract](./storage-backend-contract) says every backend family must support bootstrap and dual write
- [canonical bootstrap image](./canonical-bootstrap-image) defines the common representation used for bootstrap
- [rolling migration contract](./rolling-migration-contract) defines where validation is required in a live migration
