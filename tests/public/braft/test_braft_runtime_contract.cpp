#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "support/fs_test_util.h"
#include "support/public_braft_contract_test_util.h"

namespace {
class BraftRuntimeContractTest : public testing::Test {
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

TEST_F(BraftRuntimeContractTest, FollowerApplyReportsEperm) {
    std::vector<braft::PeerId> peers = quorumkit::test::make_peer_ids({5706, 5707, 5708});
    quorumkit::test::PublicNodeHarness node(
        "public-braft-follower-apply", peers.front(), peers);
    ASSERT_TRUE(node.start());
    ASSERT_FALSE(quorumkit::test::wait_until(1500, 50, [&] { return node.is_leader(); }));

    quorumkit::test::WaitableClosure done;
    ASSERT_TRUE(quorumkit::test::apply_string(node.node(), "follower-write", &done));
    EXPECT_EQ(EPERM, done.status().error_code()) << done.status();
}
