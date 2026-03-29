#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "braft/raft.h"

TEST(ConfigurationContract, PeerIdParsingAndFormattingMatchLegacyGrammar) {
    braft::PeerId peer_id;
    EXPECT_TRUE(peer_id.is_empty());

    EXPECT_NE(0, peer_id.parse("1.1.1.1::"));
    EXPECT_TRUE(peer_id.is_empty());

    EXPECT_EQ(0, peer_id.parse("1.1.1.1:1000:"));
    EXPECT_EQ("1.1.1.1:1000:0:0", peer_id.to_string());

    EXPECT_EQ(0, peer_id.parse("1.1.1.1:1000:0"));
    EXPECT_EQ("1.1.1.1:1000:0:0", peer_id.to_string());

    EXPECT_EQ(0, peer_id.parse("1.1.1.1:1000:0:0"));
    EXPECT_FALSE(peer_id.is_witness());
    EXPECT_EQ("1.1.1.1:1000:0:0", peer_id.to_string());

    EXPECT_EQ(0, peer_id.parse("1.1.1.1:1000:0:1"));
    EXPECT_TRUE(peer_id.is_witness());
    EXPECT_EQ("1.1.1.1:1000:0:1", peer_id.to_string());

    EXPECT_EQ(-1, peer_id.parse("1.1.1.1:1000:0:2"));

    EXPECT_EQ(0, peer_id.parse("1.1.1.1:1000"));
    EXPECT_EQ("1.1.1.1:1000:0:0", peer_id.to_string());
    const braft::PeerId copied(peer_id);
    EXPECT_EQ(peer_id, copied);

    const braft::PeerId constructed("1.2.3.4:1000:0");
    EXPECT_EQ("1.2.3.4:1000:0:0", constructed.to_string());
}

TEST(ConfigurationContract, ConfigurationHelpersStayAvailable) {
    braft::Configuration configuration;
    EXPECT_TRUE(configuration.empty());

    const std::vector<braft::PeerId> peers = {
        braft::PeerId("1.1.1.1:1000:0"),
        braft::PeerId("1.1.1.1:1000:1"),
        braft::PeerId("1.1.1.1:1000:2")};

    configuration = peers;
    EXPECT_TRUE(configuration.contains(braft::PeerId("1.1.1.1:1000:0")));
    EXPECT_FALSE(configuration.contains(braft::PeerId("1.1.1.1:2000:0")));

    std::vector<braft::PeerId> subset = {
        braft::PeerId("1.1.1.1:1000:0"),
        braft::PeerId("1.1.1.1:1000:1")};
    EXPECT_TRUE(configuration.contains(subset));
    subset.push_back(braft::PeerId("1.1.1.1:2000:1"));
    EXPECT_FALSE(configuration.contains(subset));

    EXPECT_FALSE(configuration.equals(subset));
    EXPECT_TRUE(configuration.equals(peers));

    braft::Configuration updated(peers);
    updated.remove_peer(braft::PeerId("1.1.1.1:1000:1"));
    updated.add_peer(braft::PeerId("1.1.1.1:1000:3"));

    EXPECT_FALSE(updated.contains(braft::PeerId("1.1.1.1:1000:1")));
    EXPECT_TRUE(updated.contains(braft::PeerId("1.1.1.1:1000:3")));

    std::set<braft::PeerId> peer_set;
    updated.list_peers(&peer_set);
    EXPECT_EQ(3u, peer_set.size());

    std::vector<braft::PeerId> peer_vector;
    updated.list_peers(&peer_vector);
    EXPECT_EQ(3u, peer_vector.size());
}
