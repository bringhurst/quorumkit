# Upstream braft bugs

This file documents bugs inherited from upstream braft and its required
dependency stack (primarily brpc/butil) that are worth fixing or upstreaming.
Only correctness or crashing issues go here -- not compilation portability
nits or test infrastructure problems.

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

## 2. Linux Clang crashes in file service / snapshot copying paths

**Affected tests:** `test_file_service`, `test_snapshot`,
`test_snapshot_executor`.

**Symptom:** The tests pass under GCC 15, but crash under Linux Clang 21 in the
release jobs (`clang-21-libstdcxx`, `clang-21-libcxx`). The visible failures
include:

- `brpc/socket.cpp:1954 DoWrite] Check failed: SSL_CONNECTED == ssl_state()`
- `brpc/event_dispatcher.h:228 AddConsumer] IOEvent has not been initialized`
- `std::bad_alloc`, fatal glibc mutex assertions, and SIGSEGV during snapshot
  copy / remote file copying.

**Reproduction:** Run the affected tests with the Linux Clang toolchain used in
CI:

```sh
ctest --preset conan-release --output-on-failure -R 'test_file_service|test_snapshot|test_snapshot_executor'
```

**Environment:** Ubuntu 24.04, Clang 21, C++17, brpc 1.16.0,
protobuf 3.21.12. Reproduced with both `libstdc++` and `libc++`.

**Analysis:** These failures all sit in upstream brpc/bthread server, socket,
and snapshot-copy machinery rather than in QuorumKit-specific code. The crash
sites are different, but they cluster around the same file-service and snapshot
transfer paths and do not reproduce under GCC on the same workflow.

**Workaround:** The 3 affected tests are labeled `known_clang_crash` in
`tests/CMakeLists.txt` when building on Linux with Clang, and the Clang release
CI jobs exclude them via `ctest -LE 'known_crash|known_clang_crash'`.

## 3. Linux Clang UBSan trips multiple upstream brpc/butil UB bugs

**Affected job:** `clang-21-sanitizers`.

**Affected tests:** Effectively the whole suite. In the failing CI runs,
almost every test aborted under UBSan, including `test_protobuf_file`.

**Symptom:** The sanitizer job aborts in upstream brpc/butil code before most
test logic completes. Representative failures include:

- misaligned access in `butil/files/dir_reader_linux.h`
- null-pointer-to-nonnull-parameter in `butil/logging.cc`
- misaligned `ObjectPool` / `ResourcePool` construction in
  `butil/object_pool_inl.h` and `butil/resource_pool_inl.h`
- misaligned reads in bundled MurmurHash3 code

Even `test_protobuf_file`, whose own assertions pass, still aborts under UBSan
because brpc's background bvar sampler thread hits the same misaligned
`DirReaderLinux` access during process lifetime.

There was also one QuorumKit-side UB in `src/braft/file_service.cpp`
(`FileServiceImpl::_next_id`), which has been fixed locally.

**Environment:** Ubuntu 24.04, Clang 21, ASan+UBSan, brpc 1.16.0,
protobuf 3.21.12.

**Analysis:** The failures are dominated by upstream brpc/butil UB rather than
QuorumKit logic. They are serious enough to make a full-cluster UBSan run
impractical today.

**Workaround:** In sanitizer builds, all tests except `test_ballot` are labeled
`known_sanitizer_upstream_bug` in `tests/CMakeLists.txt`, and the sanitizer CI
job excludes them via:

```sh
ctest --preset conan-debug --output-on-failure \
  -LE 'known_crash|known_clang_crash|known_sanitizer_upstream_bug'
```
