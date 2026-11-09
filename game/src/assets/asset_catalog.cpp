#include "assets/asset_catalog.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <system_error>

namespace simpsons::assets {
namespace fs = std::filesystem;
namespace {

std::string Lower(std::string_view text) {
    std::string result;
    result.reserve(text.size());
    for (const char c : text) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

bool StartsWith(const std::array<std::uint8_t, 64>& data, std::size_t size,
                std::string_view text) {
    if (size < text.size()) {
        return false;
    }
    for (std::size_t index = 0; index < text.size(); ++index) {
        if (data[index] != static_cast<std::uint8_t>(text[index])) {
            return false;
        }
    }
    return true;
}

std::string HexPreview(const std::array<std::uint8_t, 64>& data, std::size_t size) {
    std::ostringstream out;
    out << std::hex << std::uppercase << std::setfill('0');
    for (std::size_t index = 0; index < size; ++index) {
        if (index != 0) {
            out << ' ';
        }
        out << std::setw(2) << static_cast<unsigned int>(data[index]);
    }
    return out.str();
}

bool LooksLikeText(const std::array<std::uint8_t, 64>& data, std::size_t size) {
    for (std::size_t index = 0; index < size; ++index) {
        const auto c = data[index];
        if (c == 0) {
            return false;
        }
        if (c < 0x20 && c != '\n' && c != '\r' && c != '\t') {
            return false;
        }
    }
    return size > 0;
}

} // namespace

const char* AssetKindName(AssetKind kind) {
    switch (kind) {
    case AssetKind::Unknown:
        return "desconocido";
    case AssetKind::Xex:
        return "xex";
    case AssetKind::Str:
        return "str";
    case AssetKind::Pkg:
        return "pkg";
    case AssetKind::Lvl:
        return "lvl";
    case AssetKind::Costume:
        return "costume";
    case AssetKind::Lua:
        return "lua";
    case AssetKind::Cmd:
        return "cmd";
    case AssetKind::Bik:
        return "bik";
    case AssetKind::Audio:
        return "audio";
    case AssetKind::Other:
        return "otro";
    }
    return "desconocido";
}

AssetKind Classify(const fs::path& path) {
    const std::string extension = Lower(path.extension().string());
    if (extension == ".xex") {
        return AssetKind::Xex;
    }
    if (extension == ".str") {
        return AssetKind::Str;
    }
    if (extension == ".pkg") {
        return AssetKind::Pkg;
    }
    if (extension == ".lvl") {
        return AssetKind::Lvl;
    }
    if (extension == ".costume") {
        return AssetKind::Costume;
    }
    if (extension == ".lua") {
        return AssetKind::Lua;
    }
    if (extension == ".bik") {
        return AssetKind::Bik;
    }
    if (extension == ".xma" || extension == ".wav" || extension == ".wma") {
        return AssetKind::Audio;
    }
    if (Lower(path.filename().string()) == "cmd.txt") {
        return AssetKind::Cmd;
    }
    return extension.empty() ? AssetKind::Unknown : AssetKind::Other;
}

void AssetCatalog::Add(const fs::path& path) {
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) {
        return;
    }
    entries_.push_back(AssetEntry{path, Classify(path), fs::file_size(path, ec)});
}

void AssetCatalog::Scan(const fs::path& root, bool recursive, std::size_t max_entries) {
    std::error_code ec;
    if (!fs::is_directory(root, ec)) {
        return;
    }

    if (!recursive) {
        for (fs::directory_iterator it{root, ec}, end; !ec && it != end && entries_.size() < max_entries;
             it.increment(ec)) {
            Add(it->path());
        }
        return;
    }

    for (fs::recursive_directory_iterator it{root, fs::directory_options::skip_permission_denied, ec},
         end;
         !ec && it != end && entries_.size() < max_entries; it.increment(ec)) {
        Add(it->path());
    }
}

AssetSummary AssetCatalog::Summarize() const {
    AssetSummary summary;
    for (const auto& entry : entries_) {
        ++summary.counts[entry.kind];
        summary.bytes[entry.kind] += entry.size;
        ++summary.total_files;
        summary.total_bytes += entry.size;

        const std::string name = Lower(entry.path.filename().string());
        if (entry.kind == AssetKind::Xex) {
            summary.has_xex = true;
        }
        if (name == "frontend.str") {
            summary.has_frontend = true;
        }
        if (name == "cmd.txt") {
            summary.has_cmd = true;
        }
        if (entry.kind == AssetKind::Lua) {
            summary.has_lua = true;
        }
    }
    return summary;
}

std::optional<fs::path> AssetCatalog::Find(std::string_view filename) const {
    const std::string target = Lower(filename);
    for (const auto& entry : entries_) {
        if (Lower(entry.path.filename().string()) == target) {
            return entry.path;
        }
    }
    return std::nullopt;
}

AssetProbe AssetCatalog::Probe(const fs::path& path) {
    AssetProbe probe;
    probe.extension = Lower(path.extension().string());

    std::ifstream stream{path, std::ios::binary};
    if (!stream) {
        return probe;
    }
    stream.read(reinterpret_cast<char*>(probe.header.data()),
                static_cast<std::streamsize>(probe.header.size()));
    probe.header_size = static_cast<std::size_t>(stream.gcount());
    probe.hex_preview = HexPreview(probe.header, probe.header_size);
    probe.looks_text = LooksLikeText(probe.header, probe.header_size);

    if (StartsWith(probe.header, probe.header_size, "XEX2")) {
        probe.magic = "XEX2";
    } else if (StartsWith(probe.header, probe.header_size, "BIK")) {
        probe.magic = "BIK";
    } else if (probe.header_size >= 4 && probe.header[0] == 0x1B &&
               probe.header[1] == 'L' && probe.header[2] == 'u' && probe.header[3] == 'a') {
        probe.magic = "Lua bytecode";
    } else if (probe.looks_text) {
        probe.magic = "texto";
    } else {
        probe.magic = "binario";
    }
    return probe;
}

std::vector<std::string> AssetCatalog::MissingRequired(const fs::path& root) {
    AssetCatalog catalog;
    catalog.Scan(root, false);

    std::vector<std::string> missing;
    if (!catalog.Find("default.xex")) {
        missing.emplace_back("default.xex");
    }
    if (!catalog.Find("Frontend.str")) {
        missing.emplace_back("Frontend.str");
    }
    return missing;
}

} // namespace simpsons::assets
