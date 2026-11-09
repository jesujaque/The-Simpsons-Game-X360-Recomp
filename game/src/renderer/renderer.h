#pragma once

#include <string>

namespace simpsons::renderer {

struct RendererConfig {
    int width = 1920;
    int height = 1080;
    bool vsync = true;
    std::string backend = "auto";
};

/// Inicializa la capa gráfica del runtime (Vulkan/D3D12 en ReXGlue).
bool Initialize(const RendererConfig& config);

/// Presenta el frame actual.
void Present();

void Shutdown();

} // namespace simpsons::renderer
