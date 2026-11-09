#include "platform/vfs.h"

#include <algorithm>
#include <cctype>
#include <system_error>

namespace simpsons::platform {
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

std::string NormalizeDevice(std::string_view device) {
    std::string normalized = Lower(device);
    while (!normalized.empty() && (normalized.back() == ':' || normalized.back() == ' ' ||
                                   normalized.back() == '\t')) {
        normalized.pop_back();
    }
    while (!normalized.empty() && (normalized.front() == ' ' || normalized.front() == '\t')) {
        normalized.erase(normalized.begin());
    }
    return normalized;
}

fs::path AbsoluteOrOriginal(const fs::path& path) {
    std::error_code ec;
    const fs::path absolute = fs::absolute(path, ec);
    return ec ? path : absolute;
}

std::vector<std::string> SplitPath(std::string_view text) {
    std::vector<std::string> parts;
    std::size_t begin = 0;
    while (begin <= text.size()) {
        const std::size_t next = text.find('/', begin);
        const std::string_view part =
            text.substr(begin, next == std::string_view::npos ? std::string_view::npos : next - begin);
        if (!part.empty()) {
            parts.emplace_back(part);
        }
        if (next == std::string_view::npos) {
            break;
        }
        begin = next + 1;
    }
    return parts;
}

} // namespace

bool VirtualFileSystem::Mount(std::string_view device, fs::path host_root) {
    const std::string normalized = NormalizeDevice(device);
    if (normalized.empty() || host_root.empty()) {
        return false;
    }
    mounts_[normalized] = AbsoluteOrOriginal(host_root);
    return true;
}

void VirtualFileSystem::SetDefaultMount(fs::path host_root) {
    if (!host_root.empty()) {
        default_root_ = AbsoluteOrOriginal(host_root);
    }
}

std::optional<VirtualFileSystem::Resolved> VirtualFileSystem::ResolveParts(
    std::string_view game_path) const {
    std::string text{game_path};
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
        text.erase(text.begin());
    }
    while (!text.empty() && (text.back() == '\r' || text.back() == '\n' || text.back() == '\t' ||
                             text.back() == ' ')) {
        text.pop_back();
    }
    if (text.empty()) {
        return std::nullopt;
    }

    std::replace(text.begin(), text.end(), '\\', '/');

    std::optional<fs::path> root = default_root_;
    std::string_view rest = text;

    const std::size_t colon = text.find(':');
    const std::size_t slash = text.find('/');
    const bool has_device = colon != std::string::npos && (slash == std::string::npos || colon < slash);
    if (has_device) {
        const std::string device = NormalizeDevice(std::string_view{text}.substr(0, colon));
        rest = std::string_view{text}.substr(colon + 1);
        const auto mount = mounts_.find(device);
        if (mount != mounts_.end()) {
            root = mount->second;
        }
    }

    if (!root) {
        return std::nullopt;
    }

    std::vector<std::string> parts;
    for (std::string part : SplitPath(rest)) {
        if (part == ".") {
            continue;
        }
        if (part == "..") {
            if (parts.empty()) {
                // Intento de escapar de la raíz del montaje.
                return std::nullopt;
            }
            parts.pop_back();
            continue;
        }
        parts.push_back(std::move(part));
    }

    fs::path host = *root;
    for (const auto& part : parts) {
        host /= part;
    }

    return Resolved{host, *root, std::move(parts)};
}

std::optional<fs::path> VirtualFileSystem::Resolve(std::string_view game_path) const {
    const auto resolved = ResolveParts(game_path);
    if (!resolved) {
        return std::nullopt;
    }
    return resolved->host;
}

std::optional<fs::path> VirtualFileSystem::ResolveExisting(std::string_view game_path) const {
    const auto resolved = ResolveParts(game_path);
    if (!resolved) {
        return std::nullopt;
    }

    std::error_code ec;
    if (fs::exists(resolved->host, ec)) {
        return resolved->host;
    }

    fs::path current = resolved->root;
    for (const auto& part : resolved->parts) {
        const fs::path exact = current / part;
        if (fs::exists(exact, ec)) {
            current = exact;
            continue;
        }

        if (!fs::is_directory(current, ec)) {
            return std::nullopt;
        }

        const std::string target = Lower(part);
        bool found = false;
        for (fs::directory_iterator it{current, ec}, end; !ec && it != end; it.increment(ec)) {
            const std::string name = Lower(it->path().filename().string());
            if (name == target) {
                current = it->path();
                found = true;
                break;
            }
        }
        if (!found) {
            return std::nullopt;
        }
    }
    return current;
}

} // namespace simpsons::platform
