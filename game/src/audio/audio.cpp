#include "audio/audio.h"

#include <algorithm>
#include <iostream>

namespace simpsons::audio {
namespace {

struct MixerState {
    AudioState audio;
    bool initialized = false;
};

MixerState& GetState() {
    static MixerState state;
    return state;
}

float ClampVolume(float volume) {
    return std::clamp(volume, 0.0f, 1.0f);
}

} // namespace

bool Initialize(const AudioConfig& config) {
    MixerState& state = GetState();
    state.audio.enabled = config.enabled;
    state.audio.master_volume = ClampVolume(config.master_volume);
    state.audio.music_volume = ClampVolume(config.music_volume);
    state.audio.sfx_volume = ClampVolume(config.sfx_volume);
    state.initialized = true;

    if (!state.audio.enabled) {
        std::cout << "[audio] deshabilitado por configuración.\n";
        return true;
    }
    std::cout << "[audio] mixer inicializado (stub): master=" << state.audio.master_volume
              << " music=" << state.audio.music_volume << " sfx=" << state.audio.sfx_volume
              << ". Decodificador XMA pendiente del runtime.\n";
    return true;
}

void SetMasterVolume(float volume) {
    GetState().audio.master_volume = ClampVolume(volume);
}

void SetMusicVolume(float volume) {
    GetState().audio.music_volume = ClampVolume(volume);
}

void SetSfxVolume(float volume) {
    GetState().audio.sfx_volume = ClampVolume(volume);
}

AudioState State() {
    return GetState().audio;
}

void Shutdown() {
    MixerState& state = GetState();
    if (state.initialized) {
        std::cout << "[audio] cerrado.\n";
    }
    state = MixerState{};
}

} // namespace simpsons::audio
