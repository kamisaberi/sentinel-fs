#include "sentinel/fs/PathResolver.hpp"
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>

namespace sentinel::fs {

std_fs::path PathResolver::resolve_root(std::optional<std_fs::path> override_root) {
    if (override_root.has_value() && !override_root->empty()) {
        return std_fs::weakly_canonical(*override_root);
    }

    if (const char* env_root = std::getenv("SENTINEL_ROOT"); env_root != nullptr && *env_root != '\0') {
        return std_fs::weakly_canonical(std_fs::path(env_root));
    }

    if (::geteuid() == 0) {
        return std_fs::path("/var/lib/sentinel");
    }

    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        return std_fs::path(home) / ".sentinel";
    }

    return std_fs::current_path() / ".sentinel";
}

std_fs::path PathResolver::resolve_dir(ObjectClass obj_class, std::optional<std_fs::path> override_root) {
    const std_fs::path root = resolve_root(override_root);
    return root / to_subpath(obj_class);
}

std_fs::path PathResolver::resolve_pack_uri(std::string_view uri, std::optional<std_fs::path> override_root) {
    constexpr std::string_view prefix = "pack://";
    if (!uri.starts_with(prefix)) {
        throw std::invalid_argument("Invalid pack URI scheme: expected prefix 'pack://'");
    }

    const std::string path_str(uri.substr(prefix.size()));
    const auto slash = path_str.find('/');
    const std::string cartridge = (slash == std::string::npos) ? path_str : path_str.substr(0, slash);
    const std::string subpath = (slash == std::string::npos) ? "" : path_str.substr(slash + 1);

    const std_fs::path root = resolve_root(override_root);

    // Candidate 1: Installed SSFS Package
    std_fs::path candidate = root / "spkg" / "installed" / cartridge / subpath;
    if (std_fs::exists(candidate)) return std_fs::weakly_canonical(candidate);

    // Candidate 2: Global System Pack Directory
    candidate = std_fs::path("/opt/sentinel/packs") / cartridge / subpath;
    if (std_fs::exists(candidate)) return std_fs::weakly_canonical(candidate);

    // Candidate 3: Local Developer Source Tree
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        candidate = std_fs::path(home) / cartridge / subpath;
        if (std_fs::exists(candidate)) return std_fs::weakly_canonical(candidate);
    }

    return root / "spkg" / "installed" / cartridge / subpath;
}

std_fs::path PathResolver::resolve_scenario_workspace(std::string_view slug, std::optional<std::string_view> instance_id, std::optional<std_fs::path> override_root) {
    std_fs::path workspaces_dir = resolve_dir(ObjectClass::ScenarioWorkspace, override_root);
    if (instance_id.has_value() && !instance_id->empty()) {
        return workspaces_dir / (std::string(slug) + "_" + std::string(*instance_id));
    }
    return workspaces_dir / std::string(slug);
}

void PathResolver::initialize_hierarchy(std::optional<std_fs::path> override_root) {
    const std_fs::path root = resolve_root(override_root);
    const ObjectClass categories[] = {
        ObjectClass::SpkgInstalled, ObjectClass::SpkgCache, ObjectClass::SpkgStaging,
        ObjectClass::ScenarioCatalog, ObjectClass::ScenarioWorkspace, ObjectClass::ScenarioSnapshot,
        ObjectClass::BlueprintHardware, ObjectClass::BlueprintApparatus, ObjectClass::BlueprintTsn,
        ObjectClass::ModelRegistry, ObjectClass::ModelCalibrated, ObjectClass::ModelForgeCheckpoint,
        ObjectClass::SlabExperiment, ObjectClass::SlabBenchmark, ObjectClass::SlabTrajectory,
        ObjectClass::ArtifactPcap, ObjectClass::ArtifactDownload,
        ObjectClass::PkiCa, ObjectClass::PkiCerts, ObjectClass::PkiTpm,
        ObjectClass::RuntimeSocket, ObjectClass::RuntimeLock, ObjectClass::Config
    };

    for (const auto cat : categories) {
        std_fs::create_directories(resolve_dir(cat, root));
    }
}

bool PathResolver::is_contained_within(const std_fs::path& child, const std_fs::path& base) {
    const auto canonical_child = std_fs::weakly_canonical(child);
    const auto canonical_base = std_fs::weakly_canonical(base);
    auto it_base = canonical_base.begin();
    auto it_child = canonical_child.begin();

    while (it_base != canonical_base.end()) {
        if (it_child == canonical_child.end() || *it_base != *it_child) {
            return false;
        }
        ++it_base;
        ++it_child;
    }
    return true;
}

} // namespace sentinel::fs