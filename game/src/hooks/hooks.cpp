#include "hooks/hooks.h"

#include "platform/vfs.h"

#include <iostream>

namespace simpsons {

void InstallHooks(HookRegistry& registry, const HookServices& services) {
    (void)registry;
    (void)services;

    // Aquí se registrarán los hooks confirmados, por ejemplo:
    //
    //   registry.RegisterStub(0x82A71810, "boot_init_callback",
    //                         /*return_value=*/0,
    //                         "xref desde la tabla de constructores");
    //
    // No se registran las direcciones de ejemplo de game/re/addresses.md hasta
    // confirmarlas en el volcado propio. El registro vive en
    // game/functions_patch.toml para el codegen y aquí, solo si hay hook.
    std::cout << "[hooks] sin hooks de direcciones confirmadas todavía.\n";
    std::cout << "[hooks] registra stubs en src/hooks/hooks.cpp cuando triage.py detecte "
                 "llamadas no resueltas.\n";
}

std::optional<std::filesystem::path> ResolveGamePath(const HookServices& services,
                                                     std::string_view game_path,
                                                     bool must_exist) {
    if (!services.vfs) {
        return std::nullopt;
    }
    if (must_exist) {
        return services.vfs->ResolveExisting(game_path);
    }
    return services.vfs->Resolve(game_path);
}

} // namespace simpsons
