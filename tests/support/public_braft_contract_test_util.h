#ifndef QUORUMKIT_TEST_SUPPORT_PUBLIC_BRAFT_CONTRACT_TEST_UTIL_H
#define QUORUMKIT_TEST_SUPPORT_PUBLIC_BRAFT_CONTRACT_TEST_UTIL_H

#include <unistd.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <brpc/closure_guard.h>
#include <brpc/server.h>
#include <butil/endpoint.h>
#include <butil/logging.h>
#include <butil/string_printf.h>

#include "braft/raft.h"
#include "braft/snapshot.h"

namespace quorumkit::test {

inline bool wait_until(int timeout_ms, int poll_ms, const std::function<bool()>& condition) {
    const int iterations = timeout_ms / poll_ms;
    for (int i = 0; i < iterations; ++i) {
        if (condition()) {
            return true;
        }
        usleep(poll_ms * 1000);
    }
    return condition();
}

class WaitableClosure : public braft::Closure {
public:
    WaitableClosure() = default;

    void Run() override {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            finished_ = true;
        }
        cv_.notify_all();
    }

    bool wait_for(int timeout_ms) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                            [this] { return finished_; });
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool finished_ = false;
};

class RecordingStateMachine : public braft::StateMachine {
public:
    void on_apply(braft::Iterator& iter) override {
        for (; iter.valid(); iter.next()) {
            brpc::ClosureGuard done_guard(iter.done());
            std::lock_guard<std::mutex> lock(mutex_);
            applied_entries_.push_back(iter.data().to_string());
        }
    }

    void on_leader_start(int64_t term) override {
        leader_term_.store(term);
    }

    void on_leader_stop(const butil::Status&) override {
        leader_term_.store(-1);
    }

    void on_snapshot_save(braft::SnapshotWriter* writer, braft::Closure* done) override {
        brpc::ClosureGuard done_guard(done);
        const std::string snapshot_file = writer->get_path() + "/state";
        std::ofstream out(snapshot_file);
        if (!out) {
            done->status().set_error(EIO, "failed to open snapshot file");
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        for (const std::string& entry : applied_entries_) {
            out << entry << '\n';
        }
        out.close();

        if (writer->add_file("state") != 0) {
            done->status().set_error(EIO, "failed to publish snapshot file");
        }
    }

    int on_snapshot_load(braft::SnapshotReader* reader) override {
        const std::string snapshot_file = reader->get_path() + "/state";
        std::ifstream in(snapshot_file);
        if (!in) {
            return 0;
        }

        std::vector<std::string> entries;
        for (std::string line; std::getline(in, line); ) {
            entries.push_back(line);
        }

        std::lock_guard<std::mutex> lock(mutex_);
        applied_entries_ = std::move(entries);
        return 0;
    }

    size_t applied_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return applied_entries_.size();
    }

    std::string last_entry() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return applied_entries_.empty() ? std::string() : applied_entries_.back();
    }

    int64_t leader_term() const {
        return leader_term_.load();
    }

private:
    mutable std::mutex mutex_;
    std::vector<std::string> applied_entries_;
    std::atomic<int64_t> leader_term_{-1};
};

inline std::vector<braft::PeerId> make_peer_ids(std::initializer_list<int> ports) {
    butil::ip_t ip;
    CHECK_EQ(0, butil::str2ip("127.0.0.1", &ip));

    std::vector<braft::PeerId> peers;
    peers.reserve(ports.size());
    for (int port : ports) {
        peers.emplace_back(butil::EndPoint(ip, port), 0);
    }
    return peers;
}

class PublicNodeHarness {
public:
    PublicNodeHarness(std::string group_id, braft::PeerId peer_id,
                      std::vector<braft::PeerId> initial_conf)
        : group_id_(std::move(group_id))
        , peer_id_(std::move(peer_id))
        , initial_conf_(std::move(initial_conf)) {}

    ~PublicNodeHarness() {
        stop();
    }

    bool start(int election_timeout_ms = 500, int snapshot_interval_s = 3600) {
        if (started_) {
            return true;
        }

        if (braft::add_service(&server_, peer_id_.addr) != 0) {
            return false;
        }
        if (server_.Start(peer_id_.addr, nullptr) != 0) {
            return false;
        }

        braft::NodeOptions options;
        options.election_timeout_ms = election_timeout_ms;
        options.snapshot_interval_s = snapshot_interval_s;
        options.fsm = &fsm_;
        options.node_owns_fsm = false;
        options.initial_conf = braft::Configuration(initial_conf_);
        options.disable_cli = false;
        butil::string_printf(&options.log_uri, "local://./data/%s/log",
                             butil::endpoint2str(peer_id_.addr).c_str());
        butil::string_printf(&options.raft_meta_uri, "local://./data/%s/raft_meta",
                             butil::endpoint2str(peer_id_.addr).c_str());
        butil::string_printf(&options.snapshot_uri, "local://./data/%s/snapshot",
                             butil::endpoint2str(peer_id_.addr).c_str());

        node_ = std::make_unique<braft::Node>(group_id_, peer_id_);
        if (node_->init(options) != 0) {
            node_.reset();
            server_.Stop(0);
            server_.Join();
            return false;
        }

        started_ = true;
        return true;
    }

    void stop() {
        if (node_) {
            node_->shutdown(nullptr);
            node_->join();
            node_.reset();
        }
        if (started_) {
            server_.Stop(0);
            server_.Join();
            started_ = false;
        }
    }

    bool is_leader() const {
        return node_ && node_->is_leader();
    }

    braft::Node* node() const {
        return node_.get();
    }

    const braft::PeerId& peer_id() const {
        return peer_id_;
    }

    RecordingStateMachine& fsm() {
        return fsm_;
    }

private:
    std::string group_id_;
    braft::PeerId peer_id_;
    std::vector<braft::PeerId> initial_conf_;
    RecordingStateMachine fsm_;
    brpc::Server server_;
    std::unique_ptr<braft::Node> node_;
    bool started_ = false;
};

inline bool apply_string(braft::Node* node, const std::string& value,
                         WaitableClosure* done, int expected_term = -1) {
    butil::IOBuf buffer;
    buffer.append(value);

    braft::Task task;
    task.data = &buffer;
    task.done = done;
    task.expected_term = expected_term;
    node->apply(task);
    return done->wait_for(5000);
}

inline PublicNodeHarness* wait_for_single_leader(
        const std::vector<std::unique_ptr<PublicNodeHarness>>& nodes,
        int timeout_ms) {
    PublicNodeHarness* leader = nullptr;
    const bool found = wait_until(timeout_ms, 50, [&] {
        leader = nullptr;
        for (const auto& node : nodes) {
            if (node->is_leader()) {
                if (leader != nullptr) {
                    leader = nullptr;
                    return false;
                }
                leader = node.get();
            }
        }
        return leader != nullptr;
    });
    return found ? leader : nullptr;
}

inline PublicNodeHarness* wait_for_new_leader(
        const std::vector<std::unique_ptr<PublicNodeHarness>>& nodes,
        const braft::PeerId& previous_leader,
        int timeout_ms) {
    PublicNodeHarness* leader = nullptr;
    const bool found = wait_until(timeout_ms, 50, [&] {
        leader = wait_for_single_leader(nodes, 50);
        return leader != nullptr && leader->peer_id() != previous_leader;
    });
    return found ? leader : nullptr;
}

}  // namespace quorumkit::test

#endif  // QUORUMKIT_TEST_SUPPORT_PUBLIC_BRAFT_CONTRACT_TEST_UTIL_H
