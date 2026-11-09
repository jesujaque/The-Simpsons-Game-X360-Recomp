#include "patches/patches.h"

#include <iostream>

namespace simpsons {

void ApplyPatches(const PatchConfig& config) {
    if (config.widescreen) {
        std::cout << "[patches] Widescreen solicitado (pendiente de localizar la constante).\n";
    }
    if (config.skip_logos) {
        std::cout << "[patches] Skip de logos solicitado.\n";
    }
    if (config.unlock_fps) {
        std::cout << "[patches] Desbloqueo de FPS solicitado (objetivo " << config.target_fps << ").\n";
    }
}

} // namespace simpsons
