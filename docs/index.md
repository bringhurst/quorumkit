---
title: Documentation Home
sidebar_position: 1
slug: /
---

# QuorumKit Documentation

QuorumKit is a Raft library with a clean public API, an explicit braft compatibility layer, and a design that is trying to keep the core small enough to reason about clearly.

These docs now follow Diataxis.

- Tutorials help you learn by doing.
- How-to guides help you complete a task.
- Reference gives you the factual material.
- Explanation tells you why the system looks the way it does.

## Start here

If you are new to the project, this is a good reading order:

1. [Build your first cluster](./tutorials/first-cluster)
2. [Check prerequisites](./how-to/prerequisites)
3. [Build and test QuorumKit](./how-to/build-and-test)
4. [Public API overview](./reference/api-overview)
5. [Architecture overview](./architecture)

```mermaid
flowchart TD
    Home[Documentation Home] --> Tutorials[Tutorials]
    Home --> HowTo[How-To Guides]
    Home --> Reference[Reference]
    Home --> Explanation[Explanation]
    Tutorials --> FirstCluster[First Cluster]
    HowTo --> BuildTest[Build and Test]
    HowTo --> Examples[Run the Examples]
    Reference --> API[API Overview]
    Reference --> RepoMap[Repository Map]
    Explanation --> Architecture[Architecture]
    Explanation --> Runtime[Runtime]
    Explanation --> Storage[Storage]
```

## The four modes

### Tutorials

Tutorials are for learning. They are hands-on, opinionated, and meant to get you moving.

### How-to guides

How-to guides are for getting something done quickly: build the repo, run the docs, navigate the code, run the examples.

### Reference

Reference is where the facts live: API surfaces, repo map, examples, and build/deploy details.

### Explanation

Explanation is where the design lives: architecture, compatibility, runtime, transport, storage, testing, and migration.
