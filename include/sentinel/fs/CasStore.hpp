#pragma once

#include "ObjectClass.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace sentinel::fs {

namespace std_fs = std::filesystem;

struct CasObject {
    std::string sha256;
    std_fs::path path;
    std::size_t size_bytes;
};

class CasStore {
public:
    explicit CasStore(std::optional<std_fs::path> root_override = std::nullopt);

    [[nodiscard]] std::string compute_sha256(const std_fs::path& file_path) const;
    [[nodiscard]] std::optional<CasObject> lookup(std::string_view sha256) const;
    CasObject store_file(const std_fs::path& source_path, ObjectClass category = ObjectClass::ModelRegistry);
    bool verify_integrity(const CasObject& obj) const;

private:
    std_fs::path root_;
};

} // namespace sentinel::fs