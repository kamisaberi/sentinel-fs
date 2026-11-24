#pragma once

#include "ObjectClass.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace sentinel::fs {

namespace std_fs = std::filesystem;

class PathResolver {
public:
    PathResolver() = default;

    [[nodiscard]] static std_fs::path resolve_root(std::optional<std_fs::path> override_root = std::nullopt);
    [[nodiscard]] static std_fs::path resolve_dir(ObjectClass obj_class, std::optional<std_fs::path> override_root = std::nullopt);
    [[nodiscard]] static std_fs::path resolve_pack_uri(std::string_view uri, std::optional<std_fs::path> override_root = std::nullopt);
    [[nodiscard]] static std_fs::path resolve_scenario_workspace(std::string_view slug, std::optional<std::string_view> instance_id = std::nullopt, std::optional<std_fs::path> override_root = std::nullopt);

    static void initialize_hierarchy(std::optional<std_fs::path> override_root = std::nullopt);
    [[nodiscard]] static bool is_contained_within(const std_fs::path& child, const std_fs::path& base);
};

} // namespace sentinel::fs