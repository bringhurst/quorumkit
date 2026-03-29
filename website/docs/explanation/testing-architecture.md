---
title: Testing architecture
sidebar_position: 9
---

# Testing architecture

## Current state

The test directory contains 23 unit test files inherited from braft, all in `tests/`:

```text
tests/
  test_ballot.cpp              test_log_manager.cpp
  test_ballot_box.cpp          test_memory_storage.cpp
  test_checksum.cpp            test_meta.cpp
  test_cli.cpp                 test_node.cpp
  test_configuration.cpp       test_protobuf_file.cpp
  test_file_service.cpp        test_repeated_timer_task.cpp
  test_file_system_adaptor.cpp test_snapshot.cpp
  test_fsm_caller.cpp          test_snapshot_executor.cpp
  test_fsync.cpp               test_storage.cpp
  test_leader_lease.cpp        test_throttle.cpp
  test_log.cpp                 test_util.cpp
  test_log_entry.cpp
  util.h                       test helper (state machine, etc.)
  sstream_workaround.h         header shim for white-box testing
```

All tests use Google Test. They compile with `-Dprivate=public -Dprotected=public` to access library internals (white-box testing). This is inherited from upstream braft and will be replaced as the QuorumKit public API takes shape.

## Test isolation

Each test binary gets its own working directory under `testwd/<test_name>/` in the build tree. This prevents interference because many tests hardcode relative paths (`./data`) and listen ports. Tests that share port ranges are grouped with CTest `RESOURCE_LOCK` properties so they never run concurrently, even under `ctest -j`.

## Known crashes

Three tests crash deterministically due to upstream braft memory corruption bugs in configuration-change and leader-failover code paths:

- `test_node` -- SIGSEGV in `LeaderFail` with pipeline replication
- `test_cli` -- SIGSEGV in `add_and_remove_peer`
- `test_leader_lease` -- SIGSEGV in `change_peers` and `leader_remove_itself`

These are labeled `known_crash` in CMake and excluded from CI with `ctest -LE known_crash`. See `UPSTREAM_BUGS.md` for details.

## Test results

20 of 23 tests pass. The 3 known-crash tests are excluded. A full passing run:

```sh
ctest --preset conan-release --output-on-failure -LE known_crash
```

## Target state

The long-term plan is to restructure tests to mirror the source tree:

```text
tests/
  public/
    quorumkit/       tests against the QuorumKit public API
    braft_compat/    tests against the braft compatibility API
    wire_compat/     tests for mixed-version wire behavior
    storage_contract/ tests for backend bootstrap and dual write
    migration_contract/ tests for end-to-end rollout invariants
  internal/          tests for internal modules
  simulation/        deterministic simulation tests
```

The intent is to turn the contract pages in `website/docs/reference/` into executable conformance suites. The `braft` API contract, wire contract, storage contract, and rolling migration contract should all eventually map to test matrices rather than hand-wavy promises.

Simulation tests will use an in-memory transport, deterministic clock, and in-memory storage -- everything in a single process with no real I/O. This makes it possible to test network partitions, message reordering, slow disks, and clock skew deterministically.
