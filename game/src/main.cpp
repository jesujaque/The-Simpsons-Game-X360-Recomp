#include "app/application.h"
#include "platform/paths.h"

#include <iostream>
#include <string>

namespace {

void PrintUsage() {
    std::cout << "Uso: SimpsonsRecomp [opciones]\n\n"
              << "  --game-root <dir>  Datos extraídos del juego\n"
              << "  --config <file>    Configuración TOML\n"
              << "  --log-level <n>    0-3\n"
              << "  --help             Muestra esta ayuda\n";
}

} // namespace

int main(int argc, char** argv) {
    simpsons::AppOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            PrintUsage();
            return 0;
        }
        if (arg == "--game-root" && i + 1 < argc) {
            options.game_root = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            options.config = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            options.log_level = std::stoi(argv[++i]);
        } else {
            std::cerr << "Argumento desconocido: " << arg << "\n";
            PrintUsage();
            return 2;
        }
    }

    if (options.game_root.empty()) {
        const auto found = simpsons::platform::FindGameRoot();
        if (!found) {
            std::cerr << "No se encontraron los datos del juego.\n"
                      << "Usa --game-root <dir> o define SIMPSONS_GAME_ROOT.\n";
            return 1;
        }
        options.game_root = *found;
    }

    simpsons::Application app(std::move(options));
    return app.Run();
}
