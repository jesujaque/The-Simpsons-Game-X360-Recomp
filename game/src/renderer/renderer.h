#pragma once

#include <cstdint>
#include <string>

namespace simpsons::renderer {

struct RendererConfig {
    int width = 1920;
    int height = 1080;
    bool vsync = true;
    bool headless = false;
    int max_fps = 60;
    std::string backend = "auto";
    std::string aspect_ratio = "16:9";
};

struct FrameStats {
    std::uint64_t frame_count = 0;
    double last_frame_ms = 0.0;
};

/// Inicializa la capa gráfica del runtime (Vulkan/D3D12 en ReXGlue). Sin SDK
/// enlazado entra en modo "sin ventana" pero mantiene el contrato de present.
bool Initialize(const RendererConfig& config);

/// Presenta el frame actual y actualiza las estadísticas.
void Present();

void Shutdown();

bool IsInitialized();
bool IsHeadless();
FrameStats Stats();

} // namespace simpsons::renderer
