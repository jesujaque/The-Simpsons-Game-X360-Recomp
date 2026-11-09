#include "app/runtime.h"

#include "app/boot_trace.h"

#include <iostream>

#if defined(SIMPSONS_HAVE_REXGLUE_API)
std::unique_ptr<simpsons::RuntimeBackend> CreateRexGlueRuntimeBackend();
#endif

namespace simpsons {
namespace {

/// Backend de desarrollo: valida que la cadena de arranque funciona y ejecuta
/// unos frames vacíos. No pretende sustituir al juego: deja marcadas como
/// omitidas las etapas que necesitan el XEX recompilado.
class NullRuntimeBackend final : public RuntimeBackend {
public:
    const char* Name() const override { return "null"; }

    bool Initialize(const RuntimeConfig& config, BootTrace& trace) override {
        if (!config.vfs) {
            std::cerr << "[runtime] falta el sistema de archivos virtual.\n";
            return false;
        }
        std::cout << "[runtime] backend nulo (sin ReXGlue enlazado).\n";
        trace.Mark(BootStage::Entrypoint, "host preparado; falta el entrypoint recompilado");
        trace.Mark(BootStage::Crt, "constructores simulados");
        trace.Mark(BootStage::Heap, "heap simulado");
        return true;
    }

    int Run(BootTrace& trace, const FrameCallbacks& callbacks) override {
        constexpr int kSimulatedFrames = 3;
        for (int frame = 0; frame < kSimulatedFrames && !exit_requested_; ++frame) {
            if (callbacks.poll_input) {
                callbacks.poll_input();
            }
            if (callbacks.present) {
                callbacks.present();
            }
            if (callbacks.draw_ui) {
                callbacks.draw_ui();
            }
        }

        trace.Skip(BootStage::Frontend, "requiere el entrypoint recompilado del XEX");
        trace.Skip(BootStage::Gameflow, "requiere el entrypoint recompilado del XEX");
        trace.Skip(BootStage::Level, "requiere el entrypoint recompilado del XEX");
        std::cout << "[runtime] frames simulados: " << kSimulatedFrames
                  << ". Enlaza ReXGlue para arrancar el juego real.\n";
        return 0;
    }

    void RequestExit() override { exit_requested_ = true; }

    void Shutdown() override { std::cout << "[runtime] cerrado.\n"; }

private:
    bool exit_requested_ = false;
};

} // namespace

std::unique_ptr<RuntimeBackend> CreateRuntimeBackend() {
#if defined(SIMPSONS_HAVE_REXGLUE_API)
    if (auto backend = CreateRexGlueRuntimeBackend()) {
        return backend;
    }
    std::cerr << "[runtime] la integración ReXGlue no pudo crearse; usando backend nulo.\n";
#endif
    return std::make_unique<NullRuntimeBackend>();
}

} // namespace simpsons
