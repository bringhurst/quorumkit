---
title: Migrate from braft
sidebar_position: 4
---

# Migrate from braft

This page is about the practical side of migration: where to start, what to change first, and what to leave alone until later.

## Start with the public surface

Treat `quorumkit` as the destination API and `braft` as the compatibility bridge.

In practice that means:

- new code should prefer `include/quorumkit`
- existing code can stay on `include/braft` while the internals move underneath it
- migration should happen at the edges first: docs, examples, tests, then application code

## Storage migration matters too

Migration is not only about headers and namespaces. If you have existing persisted state, plan for storage migration as part of the move. QuorumKit is intended to work with the existing braft storage family while opening a path toward alternate backends.

## A sane order of operations

1. keep the existing system running through the compatibility surface
2. move new code to the QuorumKit headers
3. move examples and tests to the QuorumKit surface
4. migrate storage only when you are ready to validate it carefully

For the design rationale behind that, read [Compatibility and migration](../explanation/compatibility-and-migration).
