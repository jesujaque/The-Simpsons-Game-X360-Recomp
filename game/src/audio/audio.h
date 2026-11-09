#pragma once

#include "app/config.h"

namespace simpsons::audio {

struct AudioState {
    bool enabled = true;
    float master_volume = 1.0f;
    float music_volume = 1.0f;
    float sfx_volume = 1.0f;
};

/// Inicializa el mixer. Sin el decodificador XMA del runtime entra en modo
/// silencioso, pero conserva la configuración de volúmenes.
bool Initialize(const AudioConfig& config = {});

void SetMasterVolume(float volume);
void SetMusicVolume(float volume);
void SetSfxVolume(float volume);
AudioState State();

void Shutdown();

} // namespace simpsons::audio
