#pragma once

#include <filesystem>
#include <string>

namespace simpsons {

struct AppOptions {
    std::filesystem::path game_root;
    std::filesystem::path config;
    /// Informe Markdown de la traza de arranque. Vacío = no escribir.
    std::filesystem::path trace_out;
    /// -1 significa "usar general.log_level del archivo de configuración".
    int log_level = -1;
    bool headless = false;
    std::string backend_override;
    int fps_override = 0;
};

/// Ciclo de vida del port: valida datos, instala hooks, inicializa subsistemas
/// y cede el control al entrypoint recompilado.
class Application {
public:
    explicit Application(AppOptions options);

    int Run();

private:
    AppOptions options_;
};

} // namespace simpsons
