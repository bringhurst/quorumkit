#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "support/fs_test_util.h"
#include "support/public_braft_contract_test_util.h"

namespace {

class MigrationContractTest : public testing::Test {
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

TEST_F(MigrationContractTest, LegacyLocalUriConfigurationBootsAndRestarts) {
    const std::vector<braft::PeerId> peers = quorumkit::test::make_peer_ids({5726});

    {
        quorumkit::test::PublicNodeHarness node("public-migration-restart", peers.front(), peers);
        ASSERT_TRUE(node.start());
        ASSERT_TRUE(quorumkit::test::wait_until(5000, 50, [&] { return node.is_leader(); }));

        quorumkit::test::WaitableClosure done;
        ASSERT_TRUE(quorumkit::test::apply_string(node.node(), "persisted-entry", &done));
        ASSERT_TRUE(done.status().ok()) << done.status();
        ASSERT_TRUE(quorumkit::test::wait_until(
            5000, 50, [&] { return node.fsm().applied_count() == 1; }));
        EXPECT_EQ("persisted-entry", node.fsm().last_entry());
    }

    {
        quorumkit::test::PublicNodeHarness restarted(
            "public-migration-restart", peers.front(), peers);
        ASSERT_TRUE(restarted.start());
        ASSERT_TRUE(quorumkit::test::wait_until(5000, 50, [&] { return restarted.is_leader(); }));
        ASSERT_TRUE(quorumkit::test::wait_until(
            5000, 50, [&] { return restarted.fsm().applied_count() == 1; }));
        EXPECT_EQ("persisted-entry", restarted.fsm().last_entry());
    }
}
