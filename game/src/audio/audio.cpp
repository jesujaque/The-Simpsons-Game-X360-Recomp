#include "audio/audio.h"

#include <iostream>

namespace simpsons::audio {

bool Initialize() {
    std::cout << "[audio] mixer inicializado (stub).\n";
    // TODO(fase 3): enlazar el mixer y el decodificador XMA del runtime.
    return true;
}

void SetMasterVolume(float volume) {
    std::cout << "[audio] volumen maestro = " << volume << "\n";
}

void Shutdown() {
    std::cout << "[audio] cerrado.\n";
}

} // namespace simpsons::audio
