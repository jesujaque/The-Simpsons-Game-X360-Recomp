#include "app/application.h"

#include "audio/audio.h"
#include "hooks/hooks.h"
#include "input/input.h"
#include "patches/patches.h"
#include "platform/paths.h"
#include "renderer/renderer.h"
#include "ui/overlay.h"

#include <iostream>

namespace simpsons {

Application::Application(AppOptions options) : options_(std::move(options)) {}

int Application::Run() {
    std::cout << "[app] The Simpsons Game Recompiled\n"
              << "[app] game_root = " << options_.game_root << "\n"
              << "[app] config    = "
              << (options_.config.empty() ? "(por defecto)" : options_.config.string()) << "\n";

    if (!platform::LooksLikeGameRoot(options_.game_root)) {
        std::cerr << "[app] No se encontró default.xex en " << options_.game_root << "\n"
                  << "[app] Ejecuta primero 'python tools/bootstrap.py --check'.\n";
        return 1;
    }

    PatchConfig patches;
    patches.widescreen = true;
    patches.skip_logos = false;
    patches.unlock_fps = false;
    ApplyPatches(patches);

    InstallHooks();

    renderer::RendererConfig renderer_config;
    renderer_config.width = 1920;
    renderer_config.height = 1080;
    renderer_config.vsync = true;
    renderer_config.backend = "auto";
    if (!renderer::Initialize(renderer_config)) {
        std::cerr << "[render] No se pudo inicializar la capa gráfica.\n";
        return 1;
    }

    if (!audio::Initialize()) {
        std::cerr << "[audio] Inicialización fallida; el juego continuará sin sonido.\n";
    }

    if (!input::Initialize()) {
        std::cerr << "[input] No se detectaron mandos; se usará teclado.\n";
    }

    if (!ui::Initialize()) {
        std::cerr << "[ui] Overlay deshabilitado.\n";
    }

    // TODO(fase 2): llamar al entrypoint del código recompilado y entrar en el
    // bucle principal. El runtime de ReXGlue expone el despacho de funciones y
    // el hilo de presentación; la integración exacta depende de la versión del
    // SDK. Hasta entonces este binario solo valida la cadena de arranque.
    std::cout << "[app] Subsistemas inicializados. Falta enlazar el entrypoint recompilado.\n";

    ui::Shutdown();
    input::Shutdown();
    audio::Shutdown();
    renderer::Shutdown();
    return 0;
}

} // namespace simpsons
