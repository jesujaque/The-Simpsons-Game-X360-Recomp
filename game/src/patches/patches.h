#pragma once

#include "app/config.h"

namespace simpsons {

struct PatchConfig {
    bool widescreen = true;
    bool skip_logos = false;
    bool unlock_fps = false;
    int target_fps = 60;
};

PatchConfig PatchesFromSettings(const PatchSettings& settings, int target_fps);

/// Aplica parches de comportamiento antes de arrancar el juego.
/// No modifica el binario original: solo cambia valores en memoria o rutas.
void ApplyPatches(const PatchConfig& config);

} // namespace simpsons
