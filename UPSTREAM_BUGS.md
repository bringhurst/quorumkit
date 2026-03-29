# Upstream braft bugs

This file documents bugs inherited from upstream braft that are worth
fixing or upstreaming.  Only correctness or crashing issues go here --
not compilation portability nits or test infrastructure problems.

## 1. SEGFAULT during configuration changes (memory corruption)

**Affected tests:** `test_leader_lease` (`change_peers`, `leader_remove_itself`),
`test_cli` (`add_and_remove_peer`), `test_node` (`LeaderFail` with pipeline
replication, and others).

**Symptom:** Deterministic SIGSEGV during or shortly after Raft membership
changes.  Visible as garbled bytes in log output from `replicator.cpp:307`
(e.g. garbage where a peer address should be), followed by a crash.  The
garbled output indicates a use-after-free or buffer overrun in the replicator
during configuration transitions.

**Reproduction:**

```sh
# Build, then run in a clean temp directory:
mkdir /tmp/braft_repro && cd /tmp/braft_repro
/path/to/build/tests/test_leader_lease --gtest_filter='BaseLeaseTest.change_peers'
# → SIGSEGV every time
```

**Environment:** apple-clang 17, arm64 macOS, C++17, protobuf 3.21.12,
brpc 1.16.0, gtest 1.14.  Reproduced in both Release and Debug builds.

**Analysis:** The crash happens during `change_peers` operations that
add nodes to a Raft group one at a time.  The replicator for a newly
added peer appears to reference memory from the old configuration after
the configuration commit completes and the `ConfigurationCtx` is reset.
The bug is in the library, not the tests -- the same crash pattern
appears across three independent test binaries that all exercise
membership changes.

**Workaround:** The 3 affected test binaries are labeled `known_crash`
in `tests/CMakeLists.txt` and excluded from CI via `ctest -LE known_crash`.
The 20 remaining tests (including `test_snapshot_executor`, which passes
with clean working directories) all pass.
