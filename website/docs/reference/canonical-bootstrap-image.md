---
title: canonical bootstrap image
sidebar_position: 5
---

# canonical bootstrap image

This page defines the backend-neutral representation used to bootstrap one storage backend family from another.

The design goal is to support migration from any backend family to any other backend family without requiring custom pairwise converters.

## Purpose

If there are `n` backend families, QuorumKit wants `O(n)` import/export work, not `O(n^2)` migration adapters.

So QuorumKit standardizes on one logical export/import representation:

- every backend family exports a canonical bootstrap image
- every backend family imports a canonical bootstrap image

The canonical bootstrap image is a contract about logical contents and validation behavior. It does not require a specific physical container format. It may be stored as a file, a stream, a multipart object, or another transportable representation.

## Export cut

Every canonical bootstrap image MUST describe one well-defined export cut.

An export cut is a logical point between durable mutations. The image MUST NOT mix state from before and after the same mutation.

In practical terms, that means the image represents one coherent durable Raft state that a node could safely restart from.

## Required header information

Every canonical bootstrap image MUST carry enough metadata for validation before import, including:

- a format identifier that says this is a QuorumKit canonical bootstrap image
- a version identifier for the bootstrap image format
- the source backend family identifier
- the source backend family format version, if that family versions its durable layout separately
- checksums or equivalent integrity metadata for the image contents

Importers MUST reject images whose major format version they do not understand.

Importers MAY accept newer minor-format revisions if the image is backward-compatible and the unknown fields are explicitly ignorable.

## Required logical contents

The canonical bootstrap image MUST contain enough information to reconstruct the source backend family's durable Raft state at the export cut.

At minimum it MUST include:

- the retained log index range
- every retained logical log entry in that range
- for each retained entry: index, term, logical entry type, and payload
- persisted term and voted-for information needed for safe restart
- the latest durable snapshot metadata, including last included index and term
- the latest durable snapshot contents as exposed through the snapshot reader contract

If a backend family persists additional Raft-critical durable state beyond those items, that state MUST also round-trip through the canonical bootstrap image.

## Import rules

Import MUST be defined only for an empty destination backend instance.

Importers MUST:

- validate image integrity before accepting the image
- reject incomplete or structurally invalid images
- reject import into a non-empty destination instance
- produce deterministic durable state for a given valid input image

If an import fails, the destination instance MUST NOT be left looking like a valid successfully bootstrapped backend.

## Determinism rules

If the same valid canonical bootstrap image is imported into two empty instances of the same backend family, those two instances MUST become logically equivalent.

Logical equivalence is defined in [state equivalence and validation](./state-equivalence-and-validation).

## Extension rules

Backend families MAY carry backend-specific auxiliary information in the canonical bootstrap image if, and only if:

- that information is Raft-critical for safe restart or correct migration
- the information is clearly versioned
- other backend families are not required to reproduce backend-private internal layout exactly

The canonical bootstrap image is for durable Raft state, not for preserving every backend-private optimization artifact.

## Out of contract

This page does not require:

- a byte-identical reproduction of source on-disk layout
- pairwise custom `A -> B` migration logic
- one universal physical file format for all transports and tools

It does require one logical export/import model that all backend families implement.
