#include "app/config.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <exception>
#include <fstream>
#include <sstream>
#include <system_error>

namespace simpsons {
namespace {

std::string Trim(std::string_view text) {
    std::size_t begin = 0;
    while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])) != 0) {
        ++begin;
    }
    std::size_t end = text.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
        --end;
    }
    return std::string{text.substr(begin, end - begin)};
}

/// Elimina un comentario `#` que no esté dentro de una cadena.
std::string StripComment(std::string_view line) {
    bool in_double = false;
    bool in_single = false;
    bool escaped = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (c == '\\' && in_double) {
            escaped = true;
            continue;
        }
        if (c == '"' && !in_single) {
            in_double = !in_double;
            continue;
        }
        if (c == '\'' && !in_double) {
            in_single = !in_single;
            continue;
        }
        if (c == '#' && !in_double && !in_single) {
            return std::string{line.substr(0, i)};
        }
    }
    return std::string{line};
}

std::size_t FindUnquoted(std::string_view text, char needle) {
    bool in_double = false;
    bool in_single = false;
    bool escaped = false;
    for (std::size_t i = 0; i < text.size(); ++i) {
        const char c = text[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (c == '\\' && in_double) {
            escaped = true;
            continue;
        }
        if (c == '"' && !in_single) {
            in_double = !in_double;
            continue;
        }
        if (c == '\'' && !in_double) {
            in_single = !in_single;
            continue;
        }
        if (c == needle && !in_double && !in_single) {
            return i;
        }
    }
    return std::string_view::npos;
}

std::string Unescape(std::string_view text) {
    std::string result;
    result.reserve(text.size());
    bool escaped = false;
    for (const char c : text) {
        if (!escaped) {
            if (c == '\\') {
                escaped = true;
            } else {
                result.push_back(c);
            }
            continue;
        }
        escaped = false;
        switch (c) {
        case 'n':
            result.push_back('\n');
            break;
        case 't':
            result.push_back('\t');
            break;
        case 'r':
            result.push_back('\r');
            break;
        default:
            result.push_back(c);
            break;
        }
    }
    return result;
}

std::string ParseScalar(std::string_view raw) {
    const std::string text = Trim(raw);
    if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
        return Unescape(std::string_view{text}.substr(1, text.size() - 2));
    }
    if (text.size() >= 2 && text.front() == '\'' && text.back() == '\'') {
        return std::string{std::string_view{text}.substr(1, text.size() - 2)};
    }
    return text;
}

} // namespace

void ConfigDocument::Set(std::string key, std::string value) {
    values_[std::move(key)] = std::move(value);
}

void ConfigDocument::Error(std::string message) {
    errors_.push_back(std::move(message));
}

ConfigDocument ConfigDocument::Parse(std::string_view text) {
    ConfigDocument document;
    std::string section;

    std::size_t line_number = 0;
    std::size_t offset = 0;
    while (offset <= text.size()) {
        const std::size_t next = text.find('\n', offset);
        const std::string_view raw_line =
            text.substr(offset, next == std::string_view::npos ? std::string_view::npos : next - offset);
        offset = next == std::string_view::npos ? text.size() + 1 : next + 1;
        ++line_number;

        std::string line = Trim(StripComment(raw_line));
        if (line.empty()) {
            continue;
        }

        if (line.front() == '[') {
            if (line.back() != ']') {
                document.Error("línea " + std::to_string(line_number) + ": sección sin cerrar");
                continue;
            }
            section = Trim(std::string_view{line}.substr(1, line.size() - 2));
            if (section.empty()) {
                document.Error("línea " + std::to_string(line_number) + ": sección vacía");
            }
            continue;
        }

        const std::size_t equals = FindUnquoted(line, '=');
        if (equals == std::string::npos) {
            document.Error("línea " + std::to_string(line_number) + ": falta '='");
            continue;
        }

        std::string key = Trim(std::string_view{line}.substr(0, equals));
        const std::string value = ParseScalar(std::string_view{line}.substr(equals + 1));
        if (key.empty()) {
            document.Error("línea " + std::to_string(line_number) + ": clave vacía");
            continue;
        }
        if (value.empty()) {
            document.Error("línea " + std::to_string(line_number) + ": valor vacío para '" + key + "'");
            continue;
        }
        if (!section.empty()) {
            key = section + "." + key;
        }
        document.Set(std::move(key), value);
    }

    return document;
}

std::optional<std::string> ConfigDocument::Get(std::string_view key) const {
    const auto it = values_.find(std::string{key});
    if (it == values_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<std::string> ConfigDocument::GetString(std::string_view key) const {
    return Get(key);
}

std::optional<int> ConfigDocument::GetInt(std::string_view key) const {
    const auto value = Get(key);
    if (!value) {
        return std::nullopt;
    }
    int parsed = 0;
    const char* begin = value->data();
    const char* end = begin + value->size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc{} || result.ptr != end) {
        return std::nullopt;
    }
    return parsed;
}

std::optional<float> ConfigDocument::GetFloat(std::string_view key) const {
    const auto value = Get(key);
    if (!value) {
        return std::nullopt;
    }
    try {
        std::size_t consumed = 0;
        const float parsed = std::stof(*value, &consumed);
        if (consumed != value->size()) {
            return std::nullopt;
        }
        return parsed;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<bool> ConfigDocument::GetBool(std::string_view key) const {
    const auto value = Get(key);
    if (!value || value->empty()) {
        return std::nullopt;
    }

    std::string lowered;
    lowered.reserve(value->size());
    for (const char c : *value) {
        lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    if (lowered == "true" || lowered == "1") {
        return true;
    }
    if (lowered == "false" || lowered == "0") {
        return false;
    }
    return std::nullopt;
}

GameConfig ApplyConfig(const ConfigDocument& document) {
    GameConfig config;

    auto apply_string = [&document](std::string_view key, std::string& target) {
        if (const auto value = document.GetString(key)) {
            target = *value;
        }
    };
    auto apply_int = [&document](std::string_view key, int& target) {
        if (const auto value = document.GetInt(key)) {
            target = *value;
        }
    };
    auto apply_float = [&document](std::string_view key, float& target) {
        if (const auto value = document.GetFloat(key)) {
            target = *value;
        }
    };
    auto apply_bool = [&document](std::string_view key, bool& target) {
        if (const auto value = document.GetBool(key)) {
            target = *value;
        }
    };

    apply_string("general.game_root", config.game_root);
    apply_string("general.language", config.language);
    apply_int("general.log_level", config.log_level);

    apply_int("window.width", config.window.width);
    apply_int("window.height", config.window.height);
    apply_bool("window.fullscreen", config.window.fullscreen);
    apply_bool("window.borderless", config.window.borderless);
    apply_bool("window.vsync", config.window.vsync);
    apply_string("window.backend", config.window.backend);
    apply_string("window.aspect_ratio", config.window.aspect_ratio);

    apply_float("graphics.render_scale", config.graphics.render_scale);
    apply_int("graphics.anisotropic_filtering", config.graphics.anisotropic_filtering);
    apply_bool("graphics.fxaa", config.graphics.fxaa);
    apply_int("graphics.max_fps", config.graphics.max_fps);

    apply_bool("audio.enabled", config.audio.enabled);
    apply_float("audio.master_volume", config.audio.master_volume);
    apply_float("audio.music_volume", config.audio.music_volume);
    apply_float("audio.sfx_volume", config.audio.sfx_volume);

    apply_string("input.device", config.input.device);
    apply_float("input.deadzone", config.input.deadzone);
    apply_bool("input.invert_camera_y", config.input.invert_camera_y);

    apply_bool("patches.skip_logos", config.patches.skip_logos);
    apply_bool("patches.unlock_fps", config.patches.unlock_fps);
    apply_bool("patches.widescreen", config.patches.widescreen);

    apply_string("paths.save_dir", config.paths.save_dir);
    apply_string("paths.cache_dir", config.paths.cache_dir);

    constexpr std::string_view prefix = "input.bindings.";
    for (const auto& [key, value] : document.Values()) {
        if (key.size() > prefix.size() && key.compare(0, prefix.size(), prefix) == 0) {
            config.input.bindings[key.substr(prefix.size())] = value;
        }
    }

    if (config.window.width <= 0 || config.window.height <= 0) {
        config.warnings.push_back("window.width/height inválidos; usando 1920x1080");
        config.window.width = 1920;
        config.window.height = 1080;
    }
    if (config.graphics.render_scale <= 0.0f) {
        config.warnings.push_back("graphics.render_scale inválido; usando 1.0");
        config.graphics.render_scale = 1.0f;
    }
    if (config.graphics.max_fps < 0) {
        config.warnings.push_back("graphics.max_fps negativo; usando 60");
        config.graphics.max_fps = 60;
    }
    if (config.input.deadzone < 0.0f || config.input.deadzone > 1.0f) {
        config.warnings.push_back("input.deadzone fuera de [0,1]; usando 0.15");
        config.input.deadzone = 0.15f;
    }

    return config;
}

GameConfig LoadConfig(const std::filesystem::path& path) {
    if (path.empty()) {
        return GameConfig{};
    }

    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        GameConfig config;
        config.warnings.push_back("no existe " + path.string() + "; usando valores por defecto");
        return config;
    }

    std::ifstream stream{path, std::ios::binary};
    if (!stream) {
        GameConfig config;
        config.warnings.push_back("no se pudo abrir " + path.string() + "; usando valores por defecto");
        return config;
    }

    std::ostringstream buffer;
    buffer << stream.rdbuf();
    ConfigDocument document = ConfigDocument::Parse(buffer.str());
    GameConfig config = ApplyConfig(document);

    for (const auto& error : document.Errors()) {
        config.warnings.push_back(path.string() + ": " + error);
    }
    return config;
}

} // namespace simpsons
