#pragma once

#include "hooks/hook_registry.h"

#include <filesystem>
#include <optional>
#include <string_view>

namespace simpsons {

namespace platform {
class VirtualFileSystem;
}

struct HookServices {
    std::filesystem::path game_root;
    platform::VirtualFileSystem* vfs = nullptr;
};

/// Instala las sustituciones de funciones del juego que estén confirmadas en
/// `game/re/addresses.md`. Las direcciones de ejemplo no se registran: un hook
/// con una dirección equivocada es peor que ninguno.
void InstallHooks(HookRegistry& registry, const HookServices& services);

/// Resuelve una ruta del juego a través del sistema de archivos virtual.
/// Devuelve `nullopt` si no existe o escapa de la raíz del montaje.
std::optional<std::filesystem::path> ResolveGamePath(const HookServices& services,
                                                     std::string_view game_path,
                                                     bool must_exist = true);

} // namespace simpsons
