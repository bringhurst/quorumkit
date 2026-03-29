#include <array>

#include <gtest/gtest.h>
#include <google/protobuf/descriptor.h>

#include "braft/cli.pb.h"
#include "braft/file_service.pb.h"
#include "braft/raft.pb.h"

namespace {

void expect_field_number(const google::protobuf::Descriptor* message,
                         const char* field_name,
                         int number) {
    const auto* field = message->FindFieldByName(field_name);
    ASSERT_NE(nullptr, field) << field_name;
    EXPECT_EQ(number, field->number()) << message->full_name() << "." << field_name;
}

}  // namespace

TEST(WireContract, LegacyServiceNamesRemainStable) {
    const auto* raft_file = braft::AppendEntriesRequest::descriptor()->file();
    ASSERT_EQ(1, raft_file->service_count());
    const auto* raft_service = raft_file->service(0);
    ASSERT_EQ("RaftService", raft_service->name());
    ASSERT_EQ(5, raft_service->method_count());
    EXPECT_EQ("pre_vote", raft_service->method(0)->name());
    EXPECT_EQ("request_vote", raft_service->method(1)->name());
    EXPECT_EQ("append_entries", raft_service->method(2)->name());
    EXPECT_EQ("install_snapshot", raft_service->method(3)->name());
    EXPECT_EQ("timeout_now", raft_service->method(4)->name());

    const auto* cli_file = braft::ChangePeersRequest::descriptor()->file();
    ASSERT_EQ(1, cli_file->service_count());
    const auto* cli_service = cli_file->service(0);
    ASSERT_EQ("CliService", cli_service->name());
    ASSERT_EQ(7, cli_service->method_count());
    EXPECT_EQ("add_peer", cli_service->method(0)->name());
    EXPECT_EQ("remove_peer", cli_service->method(1)->name());
    EXPECT_EQ("change_peers", cli_service->method(2)->name());
    EXPECT_EQ("reset_peer", cli_service->method(3)->name());
    EXPECT_EQ("snapshot", cli_service->method(4)->name());
    EXPECT_EQ("get_leader", cli_service->method(5)->name());
    EXPECT_EQ("transfer_leader", cli_service->method(6)->name());

    const auto* file_service_file = braft::GetFileRequest::descriptor()->file();
    ASSERT_EQ(1, file_service_file->service_count());
    const auto* file_service = file_service_file->service(0);
    ASSERT_EQ("FileService", file_service->name());
    ASSERT_EQ(1, file_service->method_count());
    EXPECT_EQ("get_file", file_service->method(0)->name());
}

TEST(WireContract, LegacyFieldNumbersRemainStable) {
    const auto* entry_meta = braft::EntryMeta::descriptor();
    expect_field_number(entry_meta, "term", 1);
    expect_field_number(entry_meta, "type", 2);
    expect_field_number(entry_meta, "peers", 3);
    expect_field_number(entry_meta, "data_len", 4);
    expect_field_number(entry_meta, "old_peers", 5);
    EXPECT_EQ(nullptr, entry_meta->FindFieldByName("data"));

    const auto* install_snapshot = braft::InstallSnapshotRequest::descriptor();
    expect_field_number(install_snapshot, "group_id", 1);
    expect_field_number(install_snapshot, "server_id", 2);
    expect_field_number(install_snapshot, "peer_id", 3);
    expect_field_number(install_snapshot, "term", 4);
    expect_field_number(install_snapshot, "meta", 5);
    expect_field_number(install_snapshot, "uri", 6);

    const auto* timeout_now = braft::TimeoutNowRequest::descriptor();
    expect_field_number(timeout_now, "old_leader_stepped_down", 5);

    const auto* transfer_leader = braft::TransferLeaderRequest::descriptor();
    expect_field_number(transfer_leader, "group_id", 1);
    expect_field_number(transfer_leader, "leader_id", 2);
    expect_field_number(transfer_leader, "peer_id", 3);

    const auto* get_file = braft::GetFileRequest::descriptor();
    expect_field_number(get_file, "reader_id", 1);
    expect_field_number(get_file, "filename", 2);
    expect_field_number(get_file, "count", 3);
    expect_field_number(get_file, "offset", 4);
    expect_field_number(get_file, "read_partly", 5);

    const auto* get_file_response = braft::GetFileResponse::descriptor();
    expect_field_number(get_file_response, "eof", 1);
    expect_field_number(get_file_response, "read_size", 2);
    EXPECT_EQ(nullptr, get_file_response->FindFieldByName("data"));
}
