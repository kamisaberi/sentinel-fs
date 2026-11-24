#include "sentinel/fs/CasStore.hpp"
#include "sentinel/fs/PathResolver.hpp"
#include <array>
#include <fstream>
#include <iomanip>
#include <openssl/evp.h>
#include <sstream>
#include <stdexcept>

namespace sentinel::fs {

CasStore::CasStore(std::optional<std_fs::path> root_override)
    : root_(PathResolver::resolve_root(root_override)) {}

std::string CasStore::compute_sha256(const std_fs::path& file_path) const {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("CasStore: unable to open file for SHA-256 calculation: " + file_path.string());
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("CasStore: OpenSSL EVP_MD_CTX allocation failed");

    if (1 != EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("CasStore: EVP_DigestInit_ex failed");
    }

    std::array<char, 65536> buffer{};
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        if (1 != EVP_DigestUpdate(ctx, buffer.data(), static_cast<size_t>(file.gcount()))) {
            EVP_MD_CTX_free(ctx);
            throw std::runtime_error("CasStore: EVP_DigestUpdate failed");
        }
    }

    std::array<unsigned char, EVP_MAX_MD_SIZE> hash{};
    unsigned int hash_len = 0;
    if (1 != EVP_DigestFinal_ex(ctx, hash.data(), &hash_len)) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("CasStore: EVP_DigestFinal_ex failed");
    }

    EVP_MD_CTX_free(ctx);

    std::ostringstream ss;
    for (unsigned int i = 0; i < hash_len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::optional<CasObject> CasStore::lookup(std::string_view sha256) const {
    const std_fs::path registry_dir = root_ / "models" / "registry";
    const std_fs::path candidate = registry_dir / std::string(sha256);

    if (std_fs::exists(candidate) && std_fs::is_regular_file(candidate)) {
        return CasObject{
            .sha256 = std::string(sha256),
            .path = candidate,
            .size_bytes = std_fs::file_size(candidate)
        };
    }
    return std::nullopt;
}

CasObject CasStore::store_file(const std_fs::path& source_path, ObjectClass category) {
    const std::string sha256 = compute_sha256(source_path);
    const std_fs::path target_dir = PathResolver::resolve_dir(category, root_);
    std_fs::create_directories(target_dir);

    const std_fs::path dest_file = target_dir / sha256;
    if (!std_fs::exists(dest_file)) {
        std_fs::copy_file(source_path, dest_file, std_fs::copy_options::overwrite_existing);
    }

    return CasObject{
        .sha256 = sha256,
        .path = dest_file,
        .size_bytes = std_fs::file_size(dest_file)
    };
}

bool CasStore::verify_integrity(const CasObject& obj) const {
    if (!std_fs::exists(obj.path)) return false;
    return compute_sha256(obj.path) == obj.sha256;
}

} // namespace sentinel::fs