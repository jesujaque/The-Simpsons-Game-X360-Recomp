#include "hooks/hooks.h"

#include <iostream>

namespace simpsons {
namespace {

// Ejemplos de hooks que se irán añadiendo. La firma real depende de la función
// original (ver game/re/addresses.md).
//
// void Hook_GameMainLoop() { ... }
// void Hook_ContentLoadLevel(const char* path) { ... }

} // namespace

void InstallHooks() {
    // TODO(fase 3): registrar hooks en la tabla de funciones de ReXGlue.
    std::cout << "[hooks] Ningún hook instalado todavía.\n";
}

} // namespace simpsons
