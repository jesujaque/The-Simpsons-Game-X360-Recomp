#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace simpsons::platform {

/// Ruta absoluta del ejecutable en curso.
std::filesystem::path ExecutablePath();

/// Directorio de trabajo usado por defecto para los datos del juego.
std::filesystem::path DefaultGameRoot();

/// Comprueba que un directorio parece una instalación válida del juego.
bool LooksLikeGameRoot(const std::filesystem::path& path);

/// Busca los datos del juego en el hint, el directorio actual y junto al .exe.
std::optional<std::filesystem::path> FindGameRoot(const std::filesystem::path& hint = {});

/// "Windows" o "Linux" en tiempo de compilación.
std::string_view PlatformName();

} // namespace simpsons::platform
