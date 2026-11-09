#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace simpsons::platform {

/// Traduce rutas del juego (por ejemplo `d:\Frontend.str`, `device:\`,
/// `save:\profile`) al sistema de archivos del host.
///
/// El juego original usa rutas de Xbox 360, sensibles a mayúsculas solo en la
/// medida en que lo era el sistema de archivos de la consola. En Linux el
/// sistema de archivos del host sí distingue mayúsculas, así que
/// `ResolveExisting` hace una búsqueda tolerante a mayúsculas componente a
/// componente.
class VirtualFileSystem {
public:
    /// Registra un dispositivo (`d`, `device`, `save`, ...). El nombre se
    /// normaliza a minúsculas y sin `:`.
    bool Mount(std::string_view device, std::filesystem::path host_root);

    /// Raíz usada para rutas absolutas (`\foo`) y dispositivos desconocidos.
    void SetDefaultMount(std::filesystem::path host_root);

    /// Resuelve una ruta sin comprobar que exista.
    std::optional<std::filesystem::path> Resolve(std::string_view game_path) const;

    /// Resuelve y, si hace falta, corrige mayúsculas/minúsculas recorriendo el
    /// árbol real. Devuelve `nullopt` si la ruta no existe o intenta escapar
    /// de la raíz del montaje.
    std::optional<std::filesystem::path> ResolveExisting(std::string_view game_path) const;

    const std::map<std::string, std::filesystem::path>& Mounts() const { return mounts_; }
    const std::optional<std::filesystem::path>& DefaultMount() const { return default_root_; }

private:
    struct Resolved {
        std::filesystem::path host;
        std::filesystem::path root;
        std::vector<std::string> parts;
    };

    std::optional<Resolved> ResolveParts(std::string_view game_path) const;

    std::map<std::string, std::filesystem::path> mounts_;
    std::optional<std::filesystem::path> default_root_;
};

} // namespace simpsons::platform
