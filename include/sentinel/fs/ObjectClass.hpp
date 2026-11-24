#pragma once

#include <string_view>

namespace sentinel::fs {

enum class ObjectClass {
    SpkgInstalled,
    SpkgCache,
    SpkgStaging,
    ScenarioCatalog,
    ScenarioWorkspace,
    ScenarioSnapshot,
    BlueprintHardware,
    BlueprintApparatus,
    BlueprintTsn,
    ModelRegistry,
    ModelCalibrated,
    ModelForgeCheckpoint,
    SlabExperiment,
    SlabBenchmark,
    SlabTrajectory,
    ArtifactPcap,
    ArtifactDownload,
    PkiCa,
    PkiCerts,
    PkiTpm,
    RuntimeSocket,
    RuntimeLock,
    Config
};

[[nodiscard]] constexpr std::string_view to_subpath(ObjectClass obj) noexcept {
    switch (obj) {
        case ObjectClass::SpkgInstalled:       return "spkg/installed";
        case ObjectClass::SpkgCache:           return "spkg/cache";
        case ObjectClass::SpkgStaging:         return "spkg/staging";
        case ObjectClass::ScenarioCatalog:     return "scenarios/catalog";
        case ObjectClass::ScenarioWorkspace:   return "scenarios/workspaces";
        case ObjectClass::ScenarioSnapshot:    return "scenarios/snapshots";
        case ObjectClass::BlueprintHardware:   return "blueprints/hardware";
        case ObjectClass::BlueprintApparatus:  return "blueprints/apparatus";
        case ObjectClass::BlueprintTsn:        return "blueprints/tsn";
        case ObjectClass::ModelRegistry:       return "models/registry";
        case ObjectClass::ModelCalibrated:     return "models/calibrated";
        case ObjectClass::ModelForgeCheckpoint:return "models/forge/checkpoints";
        case ObjectClass::SlabExperiment:      return "slab/experiments";
        case ObjectClass::SlabBenchmark:       return "slab/benchmarks";
        case ObjectClass::SlabTrajectory:      return "slab/trajectories";
        case ObjectClass::ArtifactPcap:        return "artifacts/pcaps";
        case ObjectClass::ArtifactDownload:    return "artifacts/downloads";
        case ObjectClass::PkiCa:               return "pki/ca";
        case ObjectClass::PkiCerts:            return "pki/certs";
        case ObjectClass::PkiTpm:              return "pki/tpm";
        case ObjectClass::RuntimeSocket:       return "runtime/sockets";
        case ObjectClass::RuntimeLock:         return "runtime/locks";
        case ObjectClass::Config:              return "config";
    }
    return "";
}

} // namespace sentinel::fs