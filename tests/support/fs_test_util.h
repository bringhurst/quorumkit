#ifndef QUORUMKIT_TEST_SUPPORT_FS_TEST_UTIL_H
#define QUORUMKIT_TEST_SUPPORT_FS_TEST_UTIL_H

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

#include <gtest/gtest.h>

namespace quorumkit::test {

inline void remove_path(const std::string& path) {
    std::error_code error;
    std::filesystem::remove_all(path, error);
    ASSERT_FALSE(error) << "remove_all(" << path << ") failed: " << error.message();
}

inline void remove_children(const std::string& path) {
    const std::filesystem::path root(path);
    if (!std::filesystem::exists(root)) {
        return;
    }

    std::error_code error;
    for (const auto& entry : std::filesystem::directory_iterator(root, error)) {
        ASSERT_FALSE(error) << "directory_iterator(" << path << ") failed: " << error.message();
        std::filesystem::remove_all(entry.path(), error);
        ASSERT_FALSE(error) << "remove_all(" << entry.path().string() << ") failed: "
                            << error.message();
    }
}

inline void ensure_dir(const std::string& path) {
    std::error_code error;
    std::filesystem::create_directories(path, error);
    ASSERT_FALSE(error) << "create_directories(" << path << ") failed: " << error.message();
}

inline void touch_file(const std::string& path) {
    const std::filesystem::path file_path(path);
    if (file_path.has_parent_path()) {
        ensure_dir(file_path.parent_path().string());
    }

    const int fd = ::open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    ASSERT_GE(fd, 0) << "open(" << path << ") failed: " << std::strerror(errno);
    ASSERT_EQ(0, ::close(fd)) << "close(" << path << ") failed: " << std::strerror(errno);
}

inline void move_path(const std::string& from, const std::string& to) {
    const std::filesystem::path destination(to);
    if (destination.has_parent_path()) {
        ensure_dir(destination.parent_path().string());
    }

    std::error_code error;
    std::filesystem::rename(from, to, error);
    ASSERT_FALSE(error) << "rename(" << from << ", " << to << ") failed: "
                        << error.message();
}

}  // namespace quorumkit::test

#endif  // QUORUMKIT_TEST_SUPPORT_FS_TEST_UTIL_H
