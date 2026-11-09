#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace simpsons {

class BootTrace;
class HookRegistry;

namespace platform {
class VirtualFileSystem;
}

/// Datos que necesita el runtime para arrancar el juego recompilado.
struct RuntimeConfig {
    std::filesystem::path game_root;
    platform::VirtualFileSystem* vfs = nullptr;
    HookRegistry* hooks = nullptr;
    bool headless = false;
    int log_level = 2;
};

/// Puntos de entrada que el host presta al bucle del juego. La integración con
/// ReXGlue puede llamarlos desde su propio hilo de presentación.
struct FrameCallbacks {
    std::function<void()> poll_input;
    std::function<void()> draw_ui;
    std::function<void()> present;
};

/// Contrato entre el port y el runtime de recompilación.
///
/// `NullRuntimeBackend` permite compilar y ejecutar el esqueleto sin SDK ni
/// XEX. La implementación real (`runtime_rexglue.cpp`) se activa definiendo
/// `SIMPSONS_HAVE_REXGLUE_API` y proporcionando una fábrica
/// `CreateRexGlueRuntimeBackend()`. Ver `docs/002-primer-enlace/PHASE-2-IMPLEMENTATION.md`.
class RuntimeBackend {
public:
    virtual ~RuntimeBackend() = default;

    virtual const char* Name() const = 0;

    /// Prepara memoria, archivos y (si aplica) la ventana. Marca las etapas
    /// Entrypoint/CRT/Heap en la traza.
    virtual bool Initialize(const RuntimeConfig& config, BootTrace& trace) = 0;

    /// Ejecuta el entrypoint recompilado. Para el backend nulo simula unos
    /// frames y deja constancia de lo que falta por enlazar.
    virtual int Run(BootTrace& trace, const FrameCallbacks& callbacks) = 0;

    virtual void RequestExit() = 0;
    virtual void Shutdown() = 0;
};

std::unique_ptr<RuntimeBackend> CreateRuntimeBackend();

} // namespace simpsons
