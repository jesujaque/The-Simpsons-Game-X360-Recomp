#include "ui/overlay.h"

#include <iostream>

namespace simpsons::ui {

bool Initialize() {
    std::cout << "[ui] overlay inicializado (stub).\n";
    // TODO(fase 5): overlay de ajustes y launcher.
    return true;
}

void DrawFrame() {}

void Shutdown() {
    std::cout << "[ui] cerrado.\n";
}

} // namespace simpsons::ui
