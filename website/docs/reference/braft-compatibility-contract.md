---
title: braft compatibility contract
sidebar_position: 4
---

# braft compatibility contract

This page is the normative contract for the `include/braft/` surface.

The goal is simple: an existing service that already uses the public braft API should be able to point at QuorumKit and keep compiling without source changes, as long as it stays inside the contract described here. That includes the common case of a service that:

- constructs `braft::Node` objects directly
- subclasses `braft::StateMachine`
- proposes `braft::Task` objects
- manages snapshots through `SnapshotWriter` and `SnapshotReader`
- changes peer sets and transfers leadership
- carries a conventional C++ test suite that mocks `braft::Node`, `SnapshotWriter`, and `SnapshotReader`

If a symbol is listed here, QuorumKit should preserve its source-level shape and the documented behavior. If a symbol is not listed here, it is not part of the stable compatibility promise.

## How to read this page

- `MUST` means QuorumKit intends to preserve this as part of the compatibility contract.
- `SHOULD` means QuorumKit intends to preserve it, but it is secondary to the `MUST` surface.
- Unlisted symbols are out of contract unless another page says otherwise.

The point of the wording is practical: every `MUST` item here should eventually become a compatibility test.

## Runtime surface

### `braft/raft.h`

QuorumKit MUST provide these symbols with these source-visible shapes.

- `class braft::Closure : public google::protobuf::Closure`
  - MUST expose `butil::Status& status()`
  - MUST expose `const butil::Status& status() const`
- `struct braft::Task`
  - MUST expose public fields `butil::IOBuf* data`, `braft::Closure* done`, and `int64_t expected_term`
  - MUST default `expected_term` to `-1`
- `class braft::Iterator`
  - MUST expose `bool valid() const`
  - MUST expose `void next()`
  - MUST expose `int64_t index() const`
  - MUST expose `int64_t term() const`
  - MUST expose `const butil::IOBuf& data() const`
  - MUST expose `braft::Closure* done() const`
- `class braft::StateMachine`
  - MUST be subclassable
  - MUST expose virtual `void on_apply(::braft::Iterator&)`
  - MUST expose virtual `void on_leader_start(int64_t)`
  - MUST expose virtual `void on_leader_stop(const butil::Status&)`
  - MUST expose virtual `void on_snapshot_save(::braft::SnapshotWriter*, ::braft::Closure*)`
  - MUST expose virtual `int on_snapshot_load(::braft::SnapshotReader*)`
  - MUST expose virtual `void on_configuration_committed(const ::braft::Configuration&, int64_t)`
- `struct braft::NodeOptions`
  - MUST expose public fields `election_timeout_ms`, `snapshot_interval_s`, `catchup_margin`, `initial_conf`, `fsm`, `node_owns_fsm`, `log_uri`, `raft_meta_uri`, `snapshot_uri`, `filter_before_copy_remote`, and `disable_cli`
- `class braft::Node`
  - MUST expose constructor `Node(const GroupId&, const PeerId&)`
  - MUST expose `int init(const NodeOptions&)`
  - MUST expose `void apply(const Task&)`
  - MUST expose `PeerId leader_id()`
  - MUST expose `butil::Status list_peers(std::vector<PeerId>*)`
  - MUST expose `void change_peers(const Configuration&, Closure*)`
  - MUST expose `void shutdown(Closure*)`
  - MUST expose `int transfer_leadership_to(const PeerId&)`
- `struct braft::PeerId`
  - MUST be default constructible
  - MUST expose constructor `PeerId(butil::EndPoint, int)`
  - MUST expose public field `butil::EndPoint addr`
  - MUST expose `int parse(const std::string&)`
  - MUST expose `bool is_empty() const`
- `class braft::Configuration`
  - MUST be default constructible
  - MUST expose `bool add_peer(const PeerId&)`
  - MUST support stream output with `operator<<`
- `braft::ANY_PEER`
  - MUST exist as a `PeerId` sentinel usable with `Node::transfer_leadership_to`
- `braft::add_service`
  - MUST provide the overload `int add_service(brpc::Server*, int)`

### `braft/snapshot.h`

QuorumKit MUST provide these snapshot symbols for application code that saves and restores state through braft callbacks.

- `class braft::SnapshotWriter`
  - MUST expose `std::string get_path()`
  - MUST expose `int add_file(const std::string&)`
- `class braft::SnapshotReader`
  - MUST expose `std::string get_path()`
  - MUST expose `int load_meta(SnapshotMeta*)`
- `class braft::SnapshotMeta`
  - MUST expose `int64_t last_included_index() const`
  - MUST expose `int64_t last_included_term() const`

### `braft/util.h`

- `braft::AsyncClosureGuard`
  - MUST exist and remain usable as a scope guard around a `google::protobuf::Closure*`

### `braft/errno.pb.h`

- `braft::RaftError::ECATCHUP`
  - MUST exist as a named Raft-specific error code

## Test-facing surface

The compatibility layer is also expected to support a normal unit-test style where test doubles derive from braft interfaces directly.

These items are part of the contract too, but they are secondary to the runtime surface above.

### `braft/node.h`

- `braft::Node` SHOULD remain subclassable in tests
- The methods listed in the runtime contract SHOULD remain virtual-compatible for mocking and derived test doubles

### `braft/storage.h`

QuorumKit SHOULD preserve the abstract snapshot interface used by mock-heavy test suites.

- `class braft::SnapshotWriter`
  - SHOULD expose `void list_files(std::vector<std::string>*)`
  - SHOULD expose `int get_file_meta(const std::string&, ::google::protobuf::Message*)`
  - SHOULD expose `int save_meta(const braft::SnapshotMeta&)`
  - SHOULD expose `int add_file(const std::string&, const ::google::protobuf::Message*)`
  - SHOULD expose `int remove_file(const std::string&)`
- `class braft::SnapshotReader`
  - SHOULD expose `void list_files(std::vector<std::string>*)`
  - SHOULD expose `int get_file_meta(const std::string&, ::google::protobuf::Message*)`
  - SHOULD expose `std::string generate_uri_for_copy()`
- `class braft::SnapshotMeta`
  - SHOULD expose setters for `last_included_index` and `last_included_term`

### `braft/configuration.h`

- `braft::Configuration` SHOULD expose construction from `std::vector<PeerId>`
- `braft::Configuration` SHOULD expose `bool equals(const std::vector<PeerId>&) const`
- `braft::Configuration` SHOULD expose `bool equals(const Configuration&) const`

## Out of contract

These things are explicitly not part of the stable compatibility promise.

- direct log-segment inspection APIs such as `braft::Segment`
- configuration-history helpers such as `braft::ConfigurationManager`
- raw `LogEntry` storage layout details
- code that switches on raw numeric log entry type values
- undocumented implementation quirks that only happen to exist in upstream braft

Those APIs may still exist. They are just not the line QuorumKit promises to hold.

## Behavioral notes

Most of this contract is about source compatibility, not byte-for-byte behavioral emulation.

QuorumKit does intend to preserve a few behavior-level assumptions because existing applications commonly rely on them:

- `Closure::status()` remains the place where asynchronous result status is reported
- `Task.expected_term == -1` continues to mean "do not enforce an expected-term check"
- `ANY_PEER` continues to mean "pick a suitable follower" when transferring leadership

Beyond that, the compatibility promise is deliberately narrow. If you need a specific behavior preserved, it should be documented here so it can become a test.
