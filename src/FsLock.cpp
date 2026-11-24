#include "sentinel/fs/FsLock.hpp"
#include "sentinel/fs/PathResolver.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <sys/file.h>
#include <unistd.h>

namespace sentinel::fs {

FsLock::FsLock(std::string_view lock_name, bool blocking) {
    const std_fs::path lock_dir = PathResolver::resolve_dir(ObjectClass::RuntimeLock);
    std_fs::create_directories(lock_dir);

    lock_path_ = lock_dir / (std::string(lock_name) + ".lock");
    fd_ = ::open(lock_path_.c_str(), O_RDWR | O_CREAT, 0600);
    if (fd_ < 0) {
        throw std::runtime_error("FsLock: failed to open lock handle: " + lock_path_.string());
    }

    const int operation = blocking ? LOCK_EX : (LOCK_EX | LOCK_NB);
    if (::flock(fd_, operation) != 0) {
        ::close(fd_);
        fd_ = -1;
        throw std::runtime_error("FsLock: failed to acquire advisory lock: " + lock_path_.string());
    }
}

FsLock::~FsLock() {
    if (fd_ >= 0) {
        ::flock(fd_, LOCK_UN);
        ::close(fd_);
        fd_ = -1;
    }
}

FsLock::FsLock(FsLock&& other) noexcept
    : fd_(other.fd_), lock_path_(std::move(other.lock_path_)) {
    other.fd_ = -1;
}

FsLock& FsLock::operator=(FsLock&& other) noexcept {
    if (this != &other) {
        if (fd_ >= 0) {
            ::flock(fd_, LOCK_UN);
            ::close(fd_);
        }
        fd_ = other.fd_;
        lock_path_ = std::move(other.lock_path_);
        other.fd_ = -1;
    }
    return *this;
}

} // namespace sentinel::fs