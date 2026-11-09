#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace simpsons::input {

/// Acciones de navegación y juego que el port entiende. Los nombres coinciden
/// con las claves de `[input.bindings]` en `config.toml`.
enum class Action {
    Confirm,
    Cancel,
    Pause,
    Jump,
    Attack,
    Interact,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Count,
};

const char* ActionName(Action action);
std::optional<Action> ParseAction(std::string_view name);

/// Traduce códigos físicos (`gamepad_a`, `keyboard_return`, `key_w`, ...) a
/// acciones lógicas. Un mismo código puede disparar varias acciones, igual que
/// en la configuración de ejemplo.
class ActionMap {
public:
    void Bind(std::string code, Action action);
    bool BindFromName(std::string code, std::string_view action_name);

    std::vector<Action> ActionsFor(std::string_view code) const;
    /// Acciones activas para un conjunto de códigos pulsados.
    std::set<Action> Pressed(std::span<const std::string> codes) const;
    std::size_t Size() const;

    const std::map<std::string, std::vector<Action>>& Bindings() const { return bindings_; }

    /// Mapa por defecto con mando Xbox y teclado.
    static ActionMap Defaults();

private:
    std::map<std::string, std::vector<Action>> bindings_;
};

std::string NormalizeCode(std::string_view code);

} // namespace simpsons::input
