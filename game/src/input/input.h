#pragma once

#include "app/config.h"
#include "input/keymap.h"

#include <set>
#include <span>
#include <string>
#include <vector>

namespace simpsons::input {

/// Inicializa el backend de entrada. Sin SDL/XInput enlazado mantiene el mapa
/// de acciones y la configuración, listo para que el runtime entregue códigos.
bool Initialize(const InputConfig& config = {});

/// Bombea eventos del sistema. El backend nulo no hace nada.
void Poll();

const ActionMap& Actions();

/// Acciones activas para una lista de códigos pulsados (gamepad_a, key_w, ...).
std::set<Action> Pressed(std::span<const std::string> codes);

void Shutdown();

} // namespace simpsons::input
