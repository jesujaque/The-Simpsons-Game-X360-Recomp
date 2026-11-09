#pragma once

#include <filesystem>

namespace simpsons {

struct AppOptions {
    std::filesystem::path game_root;
    std::filesystem::path config;
    int log_level = 2;
    bool headless = false;
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
