---
title: Testing architecture
sidebar_position: 9
---

# Testing architecture

## Current state

The test tree is now split by authority instead of keeping every inherited test in one flat directory:

```text
tests/
  public/
    braft/                     public braft-facing contract tests
    storage/                   public storage contract tests
    wire/                      public wire-compat tests
    migration/                 public rollout and migration tests
  internal/
    core/                      QuorumKit implementation tests
    storage/                   internal storage implementation tests
  legacy/
    cluster/                   inherited upstream-style cluster tests
    storage/                   inherited upstream-style storage tests
  support/                     shared test helpers and shims
```

All tests use Google Test. Most of the inherited suite still compiles with `-Dprivate=public -Dprotected=public` to access library internals. That white-box access is now explicit in the directory layout: `tests/public/` is for public-facing behavior, while `tests/internal/` and most of `tests/legacy/` still lean on internals.

## Test isolation

Each test binary gets its own working directory under `testwd/<test_name>/` in the build tree. This prevents interference because many tests hardcode relative paths (`./data`) and listen ports. Tests that share port ranges are grouped with CTest `RESOURCE_LOCK` properties so they never run concurrently, even under `ctest -j`.

## Known crashes

Three tests crash deterministically due to upstream braft memory corruption bugs in configuration-change and leader-failover code paths:

- `test_node` -- SIGSEGV in `LeaderFail` with pipeline replication
- `test_cli` -- SIGSEGV in `add_and_remove_peer`
- `test_leader_lease` -- SIGSEGV in `change_peers` and `leader_remove_itself`

These are labeled `known_crash` in CMake and excluded from CI with `ctest -LE known_crash`. See `UPSTREAM_BUGS.md` for details.

## Test results

20 of the 23 inherited tests pass in the default run. The 3 known-crash tests are excluded. A full passing run:

```sh
ctest --preset conan-release --output-on-failure -LE known_crash
```

## Next step

The next step is to make `tests/public/` match the written contracts more closely:

```text
tests/
  public/
    braft/           tests against the documented braft compatibility contract
    storage/         tests against the documented storage contract
    wire/            tests for mixed-version wire behavior
    migration/       tests for end-to-end rollout invariants
  internal/          tests for internal modules and adapters
  legacy/            inherited broad regression coverage
```

The intent is to turn the contract pages in `website/docs/reference/` into executable conformance suites. The `braft` API contract, wire contract, storage contract, and rolling migration contract should all eventually map to test matrices rather than hand-wavy promises.

Deterministic simulation tests may still be added later, but they are not a prerequisite for the current public/internal/legacy split.
