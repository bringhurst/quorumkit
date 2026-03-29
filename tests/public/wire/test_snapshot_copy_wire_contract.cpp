#include <fstream>
#include <regex>
#include <string>

#include <gtest/gtest.h>

#include <brpc/server.h>
#include <butil/endpoint.h>
#include <butil/iobuf.h>

#include "braft/file_system_adaptor.h"
#include "braft/raft.h"
#include "braft/remote_file_copier.h"
#include "braft/snapshot.h"
#include "support/fs_test_util.h"

namespace {

class SnapshotCopyWireContractTest : public testing::Test {
protected:
    void SetUp() override {
        quorumkit::test::remove_path("data");
        ASSERT_EQ(0, butil::str2ip("127.0.0.1", &ip_));
        server_addr_ = butil::EndPoint(ip_, 5746);
        ASSERT_EQ(0, braft::add_service(&server_, server_addr_));
        ASSERT_EQ(0, server_.Start(server_addr_, nullptr));
    }

    void TearDown() override {
        server_.Stop(0);
        server_.Join();
        quorumkit::test::remove_path("data");
    }

    void write_file(const std::string& path, const std::string& contents) {
        const std::string parent = path.substr(0, path.find_last_of('/'));
        quorumkit::test::ensure_dir(parent);
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open()) << path;
        out << contents;
    }

    brpc::Server server_;
    butil::ip_t ip_;
    butil::EndPoint server_addr_;
};

}  // namespace

TEST_F(SnapshotCopyWireContractTest, GeneratedSnapshotUriMatchesLegacyGrammarAndCopiesBytes) {
    braft::LocalSnapshotStorage storage("./data/source");
    storage.set_server_addr(server_addr_);
    ASSERT_EQ(0, storage.init());

    std::unique_ptr<braft::SnapshotWriter> writer(storage.create());
    ASSERT_NE(nullptr, writer);

    const std::string filename = "blob.txt";
    write_file(writer->get_path() + "/" + filename, "wire payload");

    braft::SnapshotMeta meta;
    meta.set_last_included_index(8);
    meta.set_last_included_term(3);
    ASSERT_EQ(0, writer->add_file(filename));
    ASSERT_EQ(0, writer->save_meta(meta));
    ASSERT_EQ(0, storage.close(writer.release()));

    std::unique_ptr<braft::SnapshotReader> reader(storage.open());
    ASSERT_NE(nullptr, reader);

    const std::string uri = reader->generate_uri_for_copy();
    EXPECT_TRUE(std::regex_match(uri, std::regex(R"(^remote://127\.0\.0\.1:5746/[0-9]+$)")))
        << uri;

    braft::RemoteFileCopier copier;
    ASSERT_EQ(0, copier.init(uri, braft::default_file_system(), nullptr));

    butil::IOBuf data;
    ASSERT_EQ(0, copier.copy_to_iobuf(filename, &data, nullptr));
    EXPECT_EQ("wire payload", data.to_string());

    ASSERT_EQ(0, storage.close(reader.release()));
}
