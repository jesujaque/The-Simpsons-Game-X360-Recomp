#include "app/application.h"
#include "platform/paths.h"

#include <charconv>
#include <iostream>
#include <string>
#include <system_error>

namespace {

bool ParseInt(const std::string& text, int& value) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end;
}

void PrintUsage() {
    std::cout << "Uso: SimpsonsRecomp [opciones]\n\n"
              << "  --game-root <dir>  Datos extraídos del juego\n"
              << "  --config <file>    Configuración TOML\n"
              << "  --log-level <n>    0-3 (por defecto: el del config)\n"
              << "  --trace-out <file> Escribe la traza de arranque en Markdown\n"
              << "  --headless         Sin ventana; solo valida la cadena de arranque\n"
              << "  --windowed         Fuerza ventana\n"
              << "  --vulkan|--d3d12   Fuerza backend gráfico\n"
              << "  --fps <n>          Límite de presentación\n"
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
        } else if (arg == "--trace-out" && i + 1 < argc) {
            options.trace_out = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            if (!ParseInt(argv[++i], options.log_level)) {
                std::cerr << "Valor inválido para --log-level.\n";
                return 2;
            }
        } else if (arg == "--fps" && i + 1 < argc) {
            if (!ParseInt(argv[++i], options.fps_override)) {
                std::cerr << "Valor inválido para --fps.\n";
                return 2;
            }
        } else if (arg == "--headless") {
            options.headless = true;
        } else if (arg == "--windowed") {
            options.headless = false;
        } else if (arg == "--vulkan") {
            options.backend_override = "vulkan";
        } else if (arg == "--d3d12") {
            options.backend_override = "d3d12";
        } else if (arg == "--d3d11") {
            options.backend_override = "d3d11";
        } else {
            std::cerr << "Argumento desconocido: " << arg << "\n";
            PrintUsage();
            return 2;
        }
    }

    simpsons::Application app(std::move(options));
    return app.Run();
}
