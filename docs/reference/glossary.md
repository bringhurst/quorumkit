---
title: Glossary
sidebar_position: 5
---

# Glossary

## canonical API

The QuorumKit public surface under `include/quorumkit`. This is the API new code should target.

## compatibility surface

The supported `braft` public surface under `include/braft`. It preserves source compatibility for existing integrations.

## deterministic core

The part of the system that behaves as an ordered state machine rather than as a bag of concurrent callbacks.

## runtime adapter

Code that supplies time, scheduling, randomness, and execution behavior around the core.

## transport adapter

Code that moves protocol messages without defining the protocol model itself.

## storage backend

A concrete implementation of the log, metadata, or snapshot contracts.
