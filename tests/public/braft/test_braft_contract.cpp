#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <google/protobuf/message.h>

#include "braft/configuration.h"
#include "braft/node.h"
#include "braft/raft.h"
#include "braft/storage.h"
#include "braft/util.h"

namespace {

class ContractClosure : public braft::Closure {
public:
    void Run() override {}
};

class ContractStateMachine : public braft::StateMachine {
public:
    void on_apply(braft::Iterator&) override {}
    void on_leader_start(int64_t) override {}
    void on_leader_stop(const butil::Status&) override {}
    void on_snapshot_save(braft::SnapshotWriter*, braft::Closure* done) override {
        done->Run();
    }
    int on_snapshot_load(braft::SnapshotReader*) override { return 0; }
    void on_configuration_committed(const braft::Configuration&, int64_t) override {}
};

class ContractNode : public braft::Node {
public:
    ContractNode(const braft::GroupId& group_id, const braft::PeerId& peer_id)
        : braft::Node(group_id, peer_id) {}
};

template <typename T, typename = void>
struct has_add_file_with_meta : std::false_type {};

template <typename T>
struct has_add_file_with_meta<T, std::void_t<decltype(std::declval<T&>().add_file(
                                     std::declval<const std::string&>(),
                                     static_cast<const google::protobuf::Message*>(nullptr)))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_save_meta : std::false_type {};

template <typename T>
struct has_save_meta<T, std::void_t<decltype(std::declval<T&>().save_meta(
                              std::declval<const braft::SnapshotMeta&>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_list_files : std::false_type {};

template <typename T>
struct has_list_files<T, std::void_t<decltype(std::declval<T&>().list_files(
                               static_cast<std::vector<std::string>*>(nullptr)))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_get_file_meta : std::false_type {};

template <typename T>
struct has_get_file_meta<T, std::void_t<decltype(std::declval<T&>().get_file_meta(
                                   std::declval<const std::string&>(),
                                   static_cast<google::protobuf::Message*>(nullptr)))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_remove_file : std::false_type {};

template <typename T>
struct has_remove_file<T, std::void_t<decltype(std::declval<T&>().remove_file(
                                 std::declval<const std::string&>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_generate_uri_for_copy : std::false_type {};

template <typename T>
struct has_generate_uri_for_copy<T, std::void_t<decltype(std::declval<T&>().generate_uri_for_copy())>>
    : std::true_type {};

}  // namespace

TEST(BraftContract, PublicTypeSurfaceCompiles) {
    static_assert(std::is_base_of_v<google::protobuf::Closure, braft::Closure>);
    static_assert(std::is_base_of_v<braft::StateMachine, ContractStateMachine>);
    static_assert(std::is_base_of_v<braft::Node, ContractNode>);
    static_assert(std::is_constructible_v<braft::Configuration, std::vector<braft::PeerId>>);
    static_assert(has_add_file_with_meta<braft::SnapshotWriter>::value);
    static_assert(has_save_meta<braft::SnapshotWriter>::value);
    static_assert(has_list_files<braft::SnapshotWriter>::value);
    static_assert(has_get_file_meta<braft::SnapshotWriter>::value);
    static_assert(has_remove_file<braft::SnapshotWriter>::value);
    static_assert(has_list_files<braft::SnapshotReader>::value);
    static_assert(has_get_file_meta<braft::SnapshotReader>::value);
    static_assert(has_generate_uri_for_copy<braft::SnapshotReader>::value);
    static_assert(std::is_constructible_v<braft::AsyncClosureGuard, google::protobuf::Closure*>);

    ContractClosure closure;
    closure.status().set_error(EPERM, "not leader");
    EXPECT_EQ(EPERM, closure.status().error_code());
}

TEST(BraftContract, TaskAndConfigurationHelpersMatchContract) {
    braft::Task task;
    EXPECT_EQ(-1, task.expected_term);
    EXPECT_EQ(nullptr, task.data);
    EXPECT_EQ(nullptr, task.done);

    braft::NodeOptions options;
    options.election_timeout_ms = 300;
    options.snapshot_interval_s = 60;
    options.catchup_margin = 2;
    options.filter_before_copy_remote = true;
    options.disable_cli = false;

    std::vector<braft::PeerId> peers = {
        braft::PeerId("127.0.0.1:7100:0"),
        braft::PeerId("127.0.0.1:7101:0")};

    braft::Configuration configuration(peers);
    EXPECT_TRUE(configuration.equals(peers));
    EXPECT_TRUE(configuration.contains(peers[0]));

    std::vector<braft::PeerId> subset = {peers[0]};
    EXPECT_FALSE(configuration.equals(subset));

    braft::PeerId parsed;
    EXPECT_EQ(0, parsed.parse("127.0.0.1:7102"));
    EXPECT_EQ(0, parsed.parse("127.0.0.1:7102:0"));
    EXPECT_FALSE(parsed.is_empty());

    braft::SnapshotMeta meta;
    meta.set_last_included_index(12);
    meta.set_last_included_term(34);
    EXPECT_EQ(12, meta.last_included_index());
    EXPECT_EQ(34, meta.last_included_term());
}
