#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "support/fs_test_util.h"
#include "support/public_braft_contract_test_util.h"

namespace {

std::vector<std::unique_ptr<quorumkit::test::PublicNodeHarness>> make_cluster(
        const char* group_id, std::initializer_list<int> ports) {
    std::vector<braft::PeerId> peers = quorumkit::test::make_peer_ids(ports);
    std::vector<std::unique_ptr<quorumkit::test::PublicNodeHarness>> nodes;
    nodes.reserve(peers.size());
    for (const braft::PeerId& peer : peers) {
        nodes.push_back(std::make_unique<quorumkit::test::PublicNodeHarness>(group_id, peer, peers));
    }
    return nodes;
}

class BraftLeadershipTransferContractTest : public testing::Test {
protected:
    void SetUp() override {
        quorumkit::test::remove_path("data");
        GFLAGS_NS::SetCommandLineOption("raft_sync", "false");
    }

    void TearDown() override {
        quorumkit::test::remove_path("data");
    }
};

}  // namespace

TEST_F(BraftLeadershipTransferContractTest, AnyPeerLeadershipTransferMovesLeadership) {
    auto nodes = make_cluster("public-braft-leadership-transfer", {5716, 5717, 5718});
    for (const auto& node : nodes) {
        ASSERT_TRUE(node->start());
    }

    quorumkit::test::PublicNodeHarness* leader =
        quorumkit::test::wait_for_single_leader(nodes, 10000);
    ASSERT_NE(nullptr, leader);

    for (int i = 0; i < 3; ++i) {
        quorumkit::test::WaitableClosure done;
        ASSERT_TRUE(quorumkit::test::apply_string(
            leader->node(), "entry-" + std::to_string(i), &done));
        ASSERT_TRUE(done.status().ok()) << done.status();
    }

    const braft::PeerId original_leader = leader->peer_id();
    ASSERT_EQ(0, leader->node()->transfer_leadership_to(braft::ANY_PEER));

    quorumkit::test::PublicNodeHarness* new_leader =
        quorumkit::test::wait_for_new_leader(nodes, original_leader, 10000);
    ASSERT_NE(nullptr, new_leader);
    EXPECT_NE(original_leader, new_leader->peer_id());
}
