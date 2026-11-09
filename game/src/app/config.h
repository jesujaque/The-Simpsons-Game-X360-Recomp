#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace simpsons {

/// Ventana y presentación (sección [window] de config.toml).
struct WindowConfig {
    int width = 1920;
    int height = 1080;
    bool fullscreen = true;
    bool borderless = false;
    bool vsync = true;
    std::string backend = "auto";
    std::string aspect_ratio = "16:9";
};

/// Calidad gráfica (sección [graphics]).
struct GraphicsConfig {
    float render_scale = 1.0f;
    int anisotropic_filtering = 8;
    bool fxaa = true;
    int max_fps = 60;
};

/// Mezcla de audio (sección [audio]).
struct AudioConfig {
    bool enabled = true;
    float master_volume = 1.0f;
    float music_volume = 1.0f;
    float sfx_volume = 1.0f;
};

/// Entrada y rebinding (sección [input] e [input.bindings]).
struct InputConfig {
    std::string device = "auto";
    float deadzone = 0.15f;
    bool invert_camera_y = false;
    /// Acción lógica -> código físico (`confirm` -> `gamepad_a`), tal como se
    /// escribe en `[input.bindings]`.
    std::map<std::string, std::string> bindings;
};

/// Parches de comportamiento (sección [patches]).
struct PatchSettings {
    bool skip_logos = false;
    bool unlock_fps = false;
    bool widescreen = true;
};

/// Directorios de escritura (sección [paths]).
struct PathConfig {
    std::string save_dir = "saves";
    std::string cache_dir = "cache";
};

/// Configuración completa de ejecución, con valores por defecto seguros.
struct GameConfig {
    std::string game_root;
    std::string language = "en";
    int log_level = 2;

    WindowConfig window;
    GraphicsConfig graphics;
    AudioConfig audio;
    InputConfig input;
    PatchSettings patches;
    PathConfig paths;

    /// Problemas no fatales encontrados al leer el archivo.
    std::vector<std::string> warnings;
};

/// Documento TOML reducido: claves planas con notación de punto y valores
/// escalares. Es suficiente para `config.example.toml` y evita añadir una
/// dependencia externa en el arranque del port.
class ConfigDocument {
public:
    static ConfigDocument Parse(std::string_view text);

    std::optional<std::string> Get(std::string_view key) const;
    std::optional<std::string> GetString(std::string_view key) const;
    std::optional<int> GetInt(std::string_view key) const;
    std::optional<float> GetFloat(std::string_view key) const;
    std::optional<bool> GetBool(std::string_view key) const;

    const std::map<std::string, std::string>& Values() const { return values_; }
    const std::vector<std::string>& Errors() const { return errors_; }

private:
    void Set(std::string key, std::string value);
    void Error(std::string message);

    std::map<std::string, std::string> values_;
    std::vector<std::string> errors_;
};

/// Carga `config.toml`. Si la ruta está vacía o el archivo no existe devuelve
/// los valores por defecto y lo anota en `warnings`.
GameConfig LoadConfig(const std::filesystem::path& path);

/// Convierte un documento ya parseado aplicando los valores conocidos.
GameConfig ApplyConfig(const ConfigDocument& document);

} // namespace simpsons
