#include "input/input.h"

#include <iostream>

namespace simpsons::input {

bool Initialize() {
    std::cout << "[input] subsistema inicializado (stub).\n";
    // TODO(fase 3): XInput/SDL y mapeo configurable.
    return true;
}

void Poll() {}

void Shutdown() {
    std::cout << "[input] cerrado.\n";
}

} // namespace simpsons::input
