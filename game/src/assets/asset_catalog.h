#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace simpsons::assets {

enum class AssetKind {
    Unknown,
    Xex,
    Str,
    Pkg,
    Lvl,
    Costume,
    Lua,
    Cmd,
    Bik,
    Audio,
    Other,
};

const char* AssetKindName(AssetKind kind);
AssetKind Classify(const std::filesystem::path& path);

struct AssetEntry {
    std::filesystem::path path;
    AssetKind kind = AssetKind::Unknown;
    std::uintmax_t size = 0;
};

struct AssetSummary {
    std::map<AssetKind, std::size_t> counts;
    std::map<AssetKind, std::uintmax_t> bytes;
    std::size_t total_files = 0;
    std::uintmax_t total_bytes = 0;
    bool has_xex = false;
    bool has_frontend = false;
    bool has_cmd = false;
    bool has_lua = false;
};

/// Primeros bytes de un asset, para clasificarlo sin asumir un formato que
/// todavía no está documentado en `game/re/formats.md`.
struct AssetProbe {
    std::array<std::uint8_t, 64> header{};
    std::size_t header_size = 0;
    std::string extension;
    std::string magic;
    std::string hex_preview;
    bool looks_text = false;
};

/// Inventario de los datos extraídos. Se usa al arrancar para validar que el
/// frontend y los paquetes necesarios están presentes antes de llamar al
/// entrypoint recompilado.
class AssetCatalog {
public:
    void Clear() { entries_.clear(); }

    void Add(const std::filesystem::path& path);
    void Scan(const std::filesystem::path& root, bool recursive = true,
              std::size_t max_entries = 200000);

    const std::vector<AssetEntry>& Entries() const { return entries_; }
    AssetSummary Summarize() const;

    /// Busca por nombre de archivo, sin distinguir mayúsculas.
    std::optional<std::filesystem::path> Find(std::string_view filename) const;

    static AssetProbe Probe(const std::filesystem::path& path);

    /// Nombres de archivos imprescindibles que faltan para fases 2–3.
    static std::vector<std::string> MissingRequired(const std::filesystem::path& root);

private:
    std::vector<AssetEntry> entries_;
};

} // namespace simpsons::assets
