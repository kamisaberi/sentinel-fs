#pragma once

#include <filesystem>
#include <string_view>

namespace sentinel::fs {

namespace std_fs = std::filesystem;

class FsLock {
public:
    explicit FsLock(std::string_view lock_name, bool blocking = true);
    ~FsLock();

    FsLock(const FsLock&) = delete;
    FsLock& operator=(const FsLock&) = delete;
    FsLock(FsLock&& other) noexcept;
    FsLock& operator=(FsLock&& other) noexcept;

    [[nodiscard]] bool is_locked() const noexcept { return fd_ >= 0; }

private:
    int fd_{-1};
    std_fs::path lock_path_;
};

} // namespace sentinel::fs