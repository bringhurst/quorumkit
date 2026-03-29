#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <brpc/server.h>
#include <butil/endpoint.h>

#include "braft/raft.h"
#include "braft/snapshot.h"
#include "support/fs_test_util.h"

namespace {

class SnapshotStorageContractTest : public testing::Test {
protected:
    void SetUp() override {
        quorumkit::test::remove_path("data");
        ASSERT_EQ(0, butil::str2ip("127.0.0.1", &ip_));
        server_addr_ = butil::EndPoint(ip_, 5736);
        ASSERT_EQ(0, braft::add_service(&server_, server_addr_));
        ASSERT_EQ(0, server_.Start(server_addr_, nullptr));
    }

    void TearDown() override {
        server_.Stop(0);
        server_.Join();
        quorumkit::test::remove_path("data");
    }

    std::string write_snapshot_file(const std::string& root,
                                    const std::string& relative_path,
                                    const std::string& contents) {
        const std::string full_path = root + "/" + relative_path;
        const std::string parent = full_path.substr(0, full_path.find_last_of('/'));
        quorumkit::test::ensure_dir(parent);
        std::ofstream out(full_path);
        EXPECT_TRUE(out.is_open()) << full_path;
        out << contents;
        out.close();
        return full_path;
    }

    std::string read_file(const std::string& path) {
        std::ifstream in(path);
        EXPECT_TRUE(in.is_open()) << path;
        return std::string((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
    }

    brpc::Server server_;
    butil::ip_t ip_;
    butil::EndPoint server_addr_;
};

}  // namespace

TEST_F(SnapshotStorageContractTest, NestedRelativePathsRoundTripUnderReaderPath) {
    braft::LocalSnapshotStorage source("./data/source");
    source.set_server_addr(server_addr_);
    ASSERT_EQ(0, source.init());

    std::unique_ptr<braft::SnapshotWriter> writer(source.create());
    ASSERT_NE(nullptr, writer);

    const std::string relative_path = "nested/tree/state.txt";
    write_snapshot_file(writer->get_path(), relative_path, "snapshot payload");

    braft::SnapshotMeta meta;
    meta.set_last_included_index(42);
    meta.set_last_included_term(7);
    ASSERT_EQ(0, writer->add_file(relative_path));
    ASSERT_EQ(0, writer->save_meta(meta));
    ASSERT_EQ(0, source.close(writer.release()));

    std::unique_ptr<braft::SnapshotReader> source_reader(source.open());
    ASSERT_NE(nullptr, source_reader);
    std::string copy_uri = source_reader->generate_uri_for_copy();
    EXPECT_TRUE(std::regex_match(copy_uri, std::regex(R"(^remote://127\.0\.0\.1:5736/[0-9]+$)")))
        << copy_uri;

    braft::LocalSnapshotStorage destination("./data/destination");
    ASSERT_EQ(0, destination.init());

    std::unique_ptr<braft::SnapshotReader> copied_reader(destination.copy_from(copy_uri));
    ASSERT_NE(nullptr, copied_reader);

    braft::SnapshotMeta copied_meta;
    ASSERT_EQ(0, copied_reader->load_meta(&copied_meta));
    EXPECT_EQ(meta.last_included_index(), copied_meta.last_included_index());
    EXPECT_EQ(meta.last_included_term(), copied_meta.last_included_term());

    std::vector<std::string> copied_files;
    copied_reader->list_files(&copied_files);
    ASSERT_EQ(1u, copied_files.size());
    EXPECT_EQ(relative_path, copied_files.front());

    const std::string copied_path = copied_reader->get_path() + "/" + relative_path;
    EXPECT_EQ("snapshot payload", read_file(copied_path));

    ASSERT_EQ(0, source.close(source_reader.release()));
    ASSERT_EQ(0, destination.close(copied_reader.release()));
}
