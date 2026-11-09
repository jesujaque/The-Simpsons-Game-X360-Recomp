#include "patches/patches.h"

#include <iostream>

namespace simpsons {

PatchConfig PatchesFromSettings(const PatchSettings& settings, int target_fps) {
    PatchConfig config;
    config.widescreen = settings.widescreen;
    config.skip_logos = settings.skip_logos;
    config.unlock_fps = settings.unlock_fps;
    config.target_fps = target_fps;
    return config;
}

void ApplyPatches(const PatchConfig& config) {
    if (config.widescreen) {
        std::cout << "[patches] Widescreen solicitado (pendiente de localizar la constante).\n";
    }
    if (config.skip_logos) {
        std::cout << "[patches] Skip de logos solicitado.\n";
    }
    if (config.unlock_fps) {
        std::cout << "[patches] Desbloqueo de FPS solicitado (objetivo " << config.target_fps
                  << ").\n";
    }
}

} // namespace simpsons
