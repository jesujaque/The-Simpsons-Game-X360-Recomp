#include "renderer/renderer.h"

#include <iostream>

namespace simpsons::renderer {

bool Initialize(const RendererConfig& config) {
    std::cout << "[render] backend=" << config.backend << " " << config.width << "x" << config.height
              << " vsync=" << (config.vsync ? "on" : "off") << "\n";
    // TODO(fase 2): crear el dispositivo y el contexto de ReXGlue.
    return true;
}

void Present() {}

void Shutdown() {
    std::cout << "[render] cerrado.\n";
}

} // namespace simpsons::renderer
