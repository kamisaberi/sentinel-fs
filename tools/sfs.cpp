#include "sentinel/fs/CasStore.hpp"
#include "sentinel/fs/PathResolver.hpp"
#include <iostream>
#include <string_view>

using namespace sentinel::fs;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Sovereign Sentinel Filesystem (SSFS) Core Diagnostic Utility\n"
                  << "Usage: sfs <command> [args...]\n"
                  << "Commands:\n"
                  << "  init                Initialize canonical directory tree\n"
                  << "  status              Display authoritative root and core directories\n"
                  << "  resolve <pack_uri>  Resolve symbolic pack:// URI to disk path\n"
                  << "  cas-hash <file>     Compute deterministic SHA-256 hash\n";
        return 0;
    }

    const std::string_view cmd = argv[1];

    try {
        if (cmd == "init") {
            PathResolver::initialize_hierarchy();
            std::cout << "[SSFS] Hierarchy successfully initialized at: "
                      << PathResolver::resolve_root().string() << "\n";
            return 0;
        }

        if (cmd == "status") {
            const auto root = PathResolver::resolve_root();
            std::cout << "Authority Root : " << root.string() << "\n"
                      << "Scenarios Dir  : " << PathResolver::resolve_dir(ObjectClass::ScenarioCatalog).string() << "\n"
                      << "Packs Dir      : " << PathResolver::resolve_dir(ObjectClass::SpkgInstalled).string() << "\n"
                      << "Models CAS Dir : " << PathResolver::resolve_dir(ObjectClass::ModelRegistry).string() << "\n"
                      << "PCAP Dir       : " << PathResolver::resolve_dir(ObjectClass::ArtifactPcap).string() << "\n"
                      << "Runtime Sockets: " << PathResolver::resolve_dir(ObjectClass::RuntimeSocket).string() << "\n";
            return 0;
        }

        if (cmd == "resolve") {
            if (argc < 3) {
                std::cerr << "Error: 'resolve' requires a pack:// URI argument.\n";
                return 1;
            }
            const auto resolved = PathResolver::resolve_pack_uri(argv[2]);
            std::cout << resolved.string() << "\n";
            return 0;
        }

        if (cmd == "cas-hash") {
            if (argc < 3) {
                std::cerr << "Error: 'cas-hash' requires a file path argument.\n";
                return 1;
            }
            CasStore cas;
            std::cout << cas.compute_sha256(argv[2]) << "\n";
            return 0;
        }

        std::cerr << "Unknown command: " << cmd << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "SSFS Error: " << e.what() << "\n";
        return 2;
    }
}