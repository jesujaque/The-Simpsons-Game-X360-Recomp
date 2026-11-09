#include "app/application.h"

#include "app/boot_trace.h"
#include "app/config.h"
#include "app/runtime.h"
#include "assets/asset_catalog.h"
#include "audio/audio.h"
#include "hooks/hook_registry.h"
#include "hooks/hooks.h"
#include "input/input.h"
#include "patches/patches.h"
#include "platform/paths.h"
#include "platform/vfs.h"
#include "renderer/renderer.h"
#include "ui/overlay.h"

#include <iostream>
#include <system_error>

namespace simpsons {
namespace {

void PrintConfigWarnings(const GameConfig& config, int log_level) {
    if (log_level < 1) {
        return;
    }
    for (const auto& warning : config.warnings) {
        std::cerr << "[config] " << warning << "\n";
    }
}

std::filesystem::path ResolveGameRoot(const AppOptions& options, const GameConfig& config) {
    std::error_code ec;

    if (!options.game_root.empty()) {
        return std::filesystem::absolute(options.game_root, ec);
    }
    if (!config.game_root.empty()) {
        return std::filesystem::absolute(config.game_root, ec);
    }
    if (const auto found = platform::FindGameRoot()) {
        return *found;
    }
    return {};
}

} // namespace

Application::Application(AppOptions options) : options_(std::move(options)) {}

int Application::Run() {
    GameConfig config = LoadConfig(options_.config);
    const int log_level =
        options_.log_level >= 0 ? options_.log_level : config.log_level;
    PrintConfigWarnings(config, log_level);

    std::cout << "[app] The Simpsons Game Recompiled\n";
    BootTrace trace(log_level);

    const auto game_root = ResolveGameRoot(options_, config);
    if (game_root.empty() || !platform::LooksLikeGameRoot(game_root)) {
        std::cerr << "[app] No se encontró default.xex";
        if (!game_root.empty()) {
            std::cerr << " en " << game_root.string();
        }
        std::cerr << ".\n"
                  << "[app] Usa --game-root <dir> o define SIMPSONS_GAME_ROOT.\n"
                  << "[app] Ejecuta 'python tools/bootstrap.py --check' para revisar el entorno.\n";
        trace.Fail(BootStage::Entrypoint, "datos del juego ausentes");
        if (!options_.trace_out.empty()) {
            trace.WriteMarkdown(options_.trace_out);
        }
        return 1;
    }

    std::cout << "[app] game_root = " << game_root.string() << "\n";
    std::cout << "[app] config    = "
              << (options_.config.empty() ? "(por defecto)" : options_.config.string()) << "\n";

    platform::VirtualFileSystem vfs;
    vfs.Mount("d", game_root);
    vfs.Mount("device", game_root);
    vfs.Mount("game", game_root);
    vfs.SetDefaultMount(game_root);

    const auto save_dir = game_root / config.paths.save_dir;
    const auto cache_dir = game_root / config.paths.cache_dir;
    platform::EnsureDirectory(save_dir);
    platform::EnsureDirectory(cache_dir);
    vfs.Mount("save", save_dir);
    vfs.Mount("cache", cache_dir);

    std::cout << "[fs] montajes: d/device/game -> " << game_root.string() << ", save -> "
              << save_dir.string() << ", cache -> " << cache_dir.string() << "\n";

    assets::AssetCatalog catalog;
    catalog.Scan(game_root);
    const auto summary = catalog.Summarize();
    const auto missing = assets::AssetCatalog::MissingRequired(game_root);
    for (const auto& name : missing) {
        std::cerr << "[fs] falta un asset requerido: " << name << "\n";
    }
    trace.Mark(BootStage::FileSystem, std::to_string(summary.total_files) + " archivos, " +
                                          std::to_string(summary.total_bytes) + " bytes");

    if (log_level >= 3) {
        std::cout << "[fs] inventario:\n";
        for (const auto& [kind, count] : summary.counts) {
            std::cout << "      " << assets::AssetKindName(kind) << ": " << count << "\n";
        }
    }

    HookRegistry registry;
    HookServices hook_services{game_root, &vfs};
    InstallHooks(registry, hook_services);

    const PatchConfig patches =
        PatchesFromSettings(config.patches, options_.fps_override > 0 ? options_.fps_override
                                                                     : config.graphics.max_fps);
    ApplyPatches(patches);

    renderer::RendererConfig renderer_config;
    renderer_config.width = config.window.width;
    renderer_config.height = config.window.height;
    renderer_config.vsync = config.window.vsync;
    renderer_config.headless = options_.headless;
    renderer_config.max_fps = options_.fps_override > 0 ? options_.fps_override
                                                        : config.graphics.max_fps;
    renderer_config.backend =
        options_.backend_override.empty() ? config.window.backend : options_.backend_override;
    renderer_config.aspect_ratio = config.window.aspect_ratio;
    if (!renderer::Initialize(renderer_config)) {
        std::cerr << "[render] No se pudo inicializar la capa gráfica.\n";
        trace.Fail(BootStage::Render, "inicialización de la capa gráfica fallida");
        if (!options_.trace_out.empty()) {
            trace.WriteMarkdown(options_.trace_out);
        }
        return 1;
    }
    trace.Mark(BootStage::Render, renderer_config.headless ? "headless/simulado"
                                                           : renderer_config.backend);

    if (!audio::Initialize(config.audio)) {
        std::cerr << "[audio] Inicialización fallida; el juego continuará sin sonido.\n";
        trace.Fail(BootStage::Audio, "mixer no disponible");
    } else {
        trace.Mark(BootStage::Audio, config.audio.enabled ? "mixer" : "deshabilitado");
    }

    if (!input::Initialize(config.input)) {
        std::cerr << "[input] Inicialización fallida; se usará teclado por defecto.\n";
        trace.Fail(BootStage::Input, "backend de entrada no disponible");
    } else {
        trace.Mark(BootStage::Input, config.input.device);
    }

    if (!ui::Initialize()) {
        std::cerr << "[ui] Overlay deshabilitado.\n";
    }

    RuntimeConfig runtime_config;
    runtime_config.game_root = game_root;
    runtime_config.vfs = &vfs;
    runtime_config.hooks = &registry;
    runtime_config.headless = options_.headless;
    runtime_config.log_level = log_level;

    auto runtime = CreateRuntimeBackend();
    std::cout << "[runtime] backend = " << runtime->Name() << "\n";

    int exit_code = 1;
    if (!runtime->Initialize(runtime_config, trace)) {
        std::cerr << "[runtime] inicialización fallida.\n";
        trace.Fail(BootStage::Heap, "runtime no inicializado");
    } else {
        FrameCallbacks callbacks;
        callbacks.poll_input = [] { input::Poll(); };
        callbacks.draw_ui = [] { ui::DrawFrame(); };
        callbacks.present = [] { renderer::Present(); };
        exit_code = runtime->Run(trace, callbacks);
    }
    runtime->Shutdown();

    const auto frame_stats = renderer::Stats();
    std::cout << "[app] " << trace.Summary() << "\n";
    std::cout << "[app] frames presentados: " << frame_stats.frame_count << "\n";
    if (trace.Reached(BootStage::Frontend)) {
        std::cout << "[app] Menú principal alcanzado.\n";
    } else {
        std::cout << "[app] Siguiente hito: enlazar el entrypoint recompilado y llegar al "
                     "frontend (fase 3).\n";
    }

    if (registry.UnresolvedCount() > 0) {
        std::cerr << "[hooks] llamadas no resueltas: " << registry.UnresolvedCount()
                  << ". Ejecuta tools/triage.py sobre el log.\n";
    }

    if (!options_.trace_out.empty()) {
        if (trace.WriteMarkdown(options_.trace_out)) {
            std::cout << "[app] traza escrita en " << options_.trace_out.string() << "\n";
        } else {
            std::cerr << "[app] no se pudo escribir la traza en " << options_.trace_out.string()
                      << "\n";
        }
        const auto hook_report = options_.trace_out.parent_path() / "hook-report.md";
        if (registry.WriteReport(hook_report)) {
            std::cout << "[app] informe de hooks en " << hook_report.string() << "\n";
        }
    }

    ui::Shutdown();
    input::Shutdown();
    audio::Shutdown();
    renderer::Shutdown();
    return exit_code;
}

} // namespace simpsons
