#include "app/doctor.h"

#include "app/config.h"
#include "assets/asset_catalog.h"
#include "platform/paths.h"
#include "platform/vfs.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace simpsons {
namespace fs = std::filesystem;

namespace {

struct Check {
    std::string label;
    std::string detail;
    bool ok = false;
    bool required = true;
};

const char* Mark(bool ok, bool required) {
    if (ok) {
        return "[ OK ]";
    }
    return required ? "[FAIL]" : "[WARN]";
}

void PrintHeader() {
    std::cout << "==============================================================\n"
              << "  The Simpsons Game - Xbox 360 Recompiled\n"
              << "  Doctor de entorno\n"
              << "==============================================================\n";
}

void PrintUsage() {
    std::cout << "Uso: SimpsonsDoctor [opciones]\n\n"
              << "  --game-root <dir>  Directorio con default.xex y datos extraídos\n"
              << "  --config <file>    Configuración TOML a validar\n"
              << "  --strict           Devuelve código de error si falta el juego\n"
              << "  --help             Muestra esta ayuda\n";
}

} // namespace

int RunDoctor(int argc, char** argv) {
    fs::path game_root_hint;
    fs::path config_hint;
    bool strict = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            PrintUsage();
            return 0;
        }
        if (arg == "--strict") {
            strict = true;
            continue;
        }
        if (arg == "--game-root" && i + 1 < argc) {
            game_root_hint = argv[++i];
            continue;
        }
        if (arg == "--config" && i + 1 < argc) {
            config_hint = argv[++i];
            continue;
        }
        std::cerr << "Argumento desconocido: " << arg << "\n\n";
        PrintUsage();
        return 2;
    }

    PrintHeader();
    std::cout << "Plataforma: " << platform::PlatformName() << "\n\n";

    std::vector<Check> checks;

    const auto cwd = fs::current_path();
    checks.push_back({"Directorio de trabajo", cwd.string(), true, false});

    const auto game_root = platform::FindGameRoot(game_root_hint);
    checks.push_back({"Datos del juego (default.xex)",
                      game_root ? game_root->string() : "no encontrado",
                      game_root.has_value(), true});

    if (game_root) {
        const auto missing = assets::AssetCatalog::MissingRequired(*game_root);
        checks.push_back({"Frontend.str", (*game_root / "Frontend.str").string(),
                          missing.size() < 2, false});

        assets::AssetCatalog catalog;
        catalog.Scan(*game_root);
        const auto summary = catalog.Summarize();
        checks.push_back({"Inventario de assets",
                          std::to_string(summary.total_files) + " archivos, " +
                              std::to_string(summary.total_bytes) + " bytes",
                          summary.total_files > 0, false});

        platform::VirtualFileSystem vfs;
        vfs.Mount("d", *game_root);
        vfs.SetDefaultMount(*game_root);
        const auto resolved = vfs.ResolveExisting("d:\\Frontend.str");
        checks.push_back({"VFS d:\\Frontend.str",
                          resolved ? resolved->string() : "no resuelto",
                          resolved.has_value(), false});
    }

    const auto manifest = cwd / "game" / "simpsons_manifest.toml";
    checks.push_back({"Manifiesto de ReXGlue", manifest.string(), fs::exists(manifest), true});

    const auto functions_patch = cwd / "game" / "functions_patch.toml";
    checks.push_back({"Parches de funciones", functions_patch.string(),
                      fs::exists(functions_patch), false});

    const auto sdk_dir = cwd / "tools" / "rexglue-sdk";
    checks.push_back({"SDK de ReXGlue", sdk_dir.string(), fs::exists(sdk_dir), false});

    const auto generated = cwd / "game" / "generated";
    bool has_generated_code = false;
    if (fs::is_directory(generated)) {
        for (const auto& entry : fs::recursive_directory_iterator(generated)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
                has_generated_code = true;
                break;
            }
        }
    }
    checks.push_back({"Código generado", generated.string(), has_generated_code, false});

    const auto config_path = config_hint.empty() ? cwd / "config.toml" : config_hint;
    const bool has_config = fs::exists(config_path);
    if (has_config) {
        const GameConfig config = LoadConfig(config_path);
        const bool clean = config.warnings.empty();
        checks.push_back({"config.toml válido", clean ? config_path.string()
                                                       : config.warnings.front(),
                          clean, false});
    } else {
        checks.push_back({"config.toml", config_path.string() + " (se usarán defaults)", false,
                          false});
    }

    std::cout << "Comprobaciones:\n";
    for (const auto& check : checks) {
        std::cout << "  " << Mark(check.ok, check.required) << " " << check.label << "\n"
                  << "         " << check.detail << "\n";
    }

    std::cout << "\n";
    bool required_ok = true;
    for (const auto& check : checks) {
        if (check.required && !check.ok) {
            required_ok = false;
        }
    }

    if (!game_root) {
        std::cout << "Siguiente paso:\n"
                  << "  1. Consigue una copia legal del juego para Xbox 360.\n"
                  << "  2. python tools/extract_iso.py <ruta-al-iso> --out game/extracted\n"
                  << "  3. Repite este diagnóstico.\n";
    } else if (!has_generated_code) {
        std::cout << "Siguiente paso:\n"
                  << "  python tools/codegen.py --sdk tools/rexglue-sdk\n";
    } else {
        std::cout << "Todo listo. Compila con:\n"
                  << "  cmake --preset windows-full && cmake --build --preset windows-full\n";
    }

    if (!strict) {
        return 0;
    }
    return required_ok ? 0 : 1;
}

} // namespace simpsons
