---
title: wire compatibility contract
sidebar_position: 3
---

# wire compatibility contract

This page is the normative contract for mixed-version clusters and tooling that speak the legacy braft wire protocol.

QuorumKit may refactor transport internals, but a rolling migration only works if old nodes and new nodes can still talk to each other. For that reason, the default migration path MUST preserve the legacy braft wire dialect.

## Scope

This contract covers:

- node-to-node Raft RPCs
- snapshot file transfer RPCs
- administrative RPCs that existing operators and tools may still call during migration
- the attachment and string-encoding conventions those RPCs rely on

This contract does not require QuorumKit to use brpc internally forever. It does require any compatibility transport mode to look the same on the wire as existing braft deployments.

## Stable services and methods

QuorumKit MUST preserve the externally visible service and method names in these schemas:

- `src/braft/raft.proto`
  - `RaftService.pre_vote`
  - `RaftService.request_vote`
  - `RaftService.append_entries`
  - `RaftService.install_snapshot`
  - `RaftService.timeout_now`
- `src/braft/file_service.proto`
  - `FileService.get_file`
- `src/braft/cli.proto`
  - `CliService.add_peer`
  - `CliService.remove_peer`
  - `CliService.change_peers`
  - `CliService.reset_peer`
  - `CliService.snapshot`
  - `CliService.get_leader`
  - `CliService.transfer_leader`

QuorumKit SHOULD preserve the operational compatibility surface in `src/braft/builtin_service.proto`, including `raft_stat.default_method`, for drop-in observability and tooling.

## Schema stability rules

For the protobuf messages in the legacy wire schemas:

- existing field numbers MUST NOT be renumbered
- existing fields MUST NOT be removed
- existing fields MUST NOT be repurposed to a different meaning
- existing field types and cardinality (`required`, `optional`, `repeated`) MUST remain wire-compatible

QuorumKit MAY add new optional fields if, and only if:

- legacy peers can ignore them safely
- mixed-version behavior remains correct when old nodes never send or read them

## Attachment conventions

Some parts of the wire contract are not expressed entirely in protobuf fields.

QuorumKit MUST preserve these conventions:

- `AppendEntries` entry payload bytes remain carried in the RPC attachment, with `EntryMeta.data_len` describing the attachment-side payload length
- `GetFileResponse` file bytes remain carried in the RPC attachment

Changing those conventions would be a wire break even if the protobuf schemas were unchanged.

## String and URI conventions

QuorumKit MUST preserve these compatibility encodings:

- `peer_id` and `server_id` strings use the legacy `PeerId::to_string()` grammar, including the optional peer-index suffix used by existing deployments (`host:port[:idx]`)
- `group_id` remains the same application-level identifier used by legacy deployments
- snapshot-copy URIs carried in `InstallSnapshotRequest.uri` remain consumable by legacy snapshot download logic built around `FileService`, including the legacy `remote://<server_addr>/<reader_id>` grammar emitted by `SnapshotReader::generate_uri_for_copy()`

## Mixed-version cluster guarantees

In a supported rolling migration, a cluster containing both legacy braft nodes and QuorumKit nodes MUST be able to:

- elect a leader
- replicate and commit logs
- install snapshots
- transfer leadership
- process peer configuration changes through the legacy RPC/admin surface

Storage backend selection MUST NOT change the network-visible protocol. A node using one backend family and a node using another backend family still have to speak the same legacy wire dialect during migration.

## Transport independence

This contract freezes the wire behavior, not the implementation behind it.

QuorumKit MAY eventually support:

- a different transport implementation that still speaks the legacy braft wire dialect
- a separate native QuorumKit wire protocol

But a native QuorumKit wire protocol, if it exists, MUST be treated as a separate explicit mode. It is not the default rolling-migration path.

## Out of contract

This contract does not require:

- permanent brpc internals inside QuorumKit
- identical connection pooling or threading behavior
- identical performance characteristics

It does require that old peers, new peers, and existing operational tools continue to interoperate across a mixed-version rollout.
