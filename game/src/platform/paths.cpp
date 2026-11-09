#include "platform/paths.h"

#include <cstdlib>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace simpsons::platform {
namespace fs = std::filesystem;

fs::path ExecutablePath() {
#if defined(_WIN32)
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
        return fs::path{buffer};
    }
    return {};
#else
    std::error_code ec;
    auto path = fs::read_symlink("/proc/self/exe", ec);
    return ec ? fs::path{} : path;
#endif
}

fs::path DefaultGameRoot() {
    return fs::current_path() / "game" / "extracted";
}

bool LooksLikeGameRoot(const fs::path& path) {
    std::error_code ec;
    if (!fs::is_directory(path, ec)) {
        return false;
    }
    return fs::exists(path / "default.xex", ec);
}

std::optional<fs::path> FindGameRoot(const fs::path& hint) {
    if (!hint.empty() && LooksLikeGameRoot(hint)) {
        return fs::absolute(hint);
    }

    if (const char* env = std::getenv("SIMPSONS_GAME_ROOT"); env != nullptr && *env != '\0') {
        const fs::path from_env{env};
        if (LooksLikeGameRoot(from_env)) {
            return fs::absolute(from_env);
        }
    }

    const auto cwd_root = DefaultGameRoot();
    if (LooksLikeGameRoot(cwd_root)) {
        return fs::absolute(cwd_root);
    }

    const auto exe_dir = ExecutablePath().parent_path();
    for (const auto& candidate : {exe_dir / "game" / "extracted", exe_dir / "extracted", exe_dir}) {
        if (LooksLikeGameRoot(candidate)) {
            return fs::absolute(candidate);
        }
    }

    return std::nullopt;
}

std::string_view PlatformName() {
#if defined(_WIN32)
    return "Windows";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

} // namespace simpsons::platform
