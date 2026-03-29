/*
 * =====================================================================================
 *
 *       Filename:  test_configuration.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年10月22日 15时16分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  WangYao (fisherman), wangyao02@baidu.com
 *        Company:  Baidu, Inc
 *
 * =====================================================================================
 */

#include <gtest/gtest.h>

#include "braft/raft.h"
#include "braft/configuration_manager.h"

TEST(ConfigurationManagerTest, TracksLastConfigurationAcrossTruncation) {
    braft::ConfigurationManager conf_manager;

    braft::ConfigurationEntry it1;
    conf_manager.get(10, &it1);
    ASSERT_EQ(it1.id, braft::LogId(0, 0));
    ASSERT_TRUE(it1.conf.empty());
    ASSERT_EQ(braft::LogId(0, 0), conf_manager.last_configuration().id);
    braft::ConfigurationEntry entry;
    std::vector<braft::PeerId> peers;
    peers.push_back(braft::PeerId("1.1.1.1:1000:0"));
    peers.push_back(braft::PeerId("1.1.1.1:1000:1"));
    peers.push_back(braft::PeerId("1.1.1.1:1000:2"));
    entry.conf = peers;
    entry.id = braft::LogId(8, 1);
    conf_manager.add(entry);
    ASSERT_EQ(braft::LogId(8, 1), conf_manager.last_configuration().id);

    conf_manager.get(10, &it1);
    ASSERT_EQ(it1.id, entry.id);

    conf_manager.truncate_suffix(7);
    ASSERT_EQ(braft::LogId(0, 0), conf_manager.last_configuration().id);

    entry.id = braft::LogId(10, 1);
    entry.conf = peers;
    conf_manager.add(entry);
    peers.push_back(braft::PeerId("1.1.1.1:1000:3"));
    entry.id = braft::LogId(20, 1);
    entry.conf = peers;
    conf_manager.add(entry);
    ASSERT_EQ(braft::LogId(20, 1), conf_manager.last_configuration().id);

    conf_manager.truncate_prefix(15);
    ASSERT_EQ(braft::LogId(20, 1), conf_manager.last_configuration().id);

    conf_manager.truncate_prefix(25);
    ASSERT_EQ(braft::LogId(0, 0), conf_manager.last_configuration().id);

}
