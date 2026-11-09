#include "input/input.h"

#include <iostream>

namespace simpsons::input {
namespace {

struct State {
    ActionMap actions;
    InputConfig config;
    bool initialized = false;
};

State& GetState() {
    static State state;
    return state;
}

} // namespace

bool Initialize(const InputConfig& config) {
    State& state = GetState();
    state.config = config;
    state.actions = ActionMap::Defaults();

    std::size_t applied = 0;
    for (const auto& [action_name, code] : config.bindings) {
        if (state.actions.BindFromName(code, action_name)) {
            ++applied;
        } else {
            std::cerr << "[input] acción desconocida '" << action_name << "' para '" << code
                      << "'.\n";
        }
    }

    state.initialized = true;
    std::cout << "[input] mapa con " << state.actions.Size() << " bindings (" << applied
              << " desde config), dispositivo=" << config.device
              << ", deadzone=" << config.deadzone << ".\n";
    std::cout << "[input] backend nulo: el runtime debe entregar códigos físicos.\n";
    return true;
}

void Poll() {
    // El backend real (SDL/XInput) llegará con la integración de ReXGlue.
}

const ActionMap& Actions() {
    return GetState().actions;
}

std::set<Action> Pressed(std::span<const std::string> codes) {
    return GetState().actions.Pressed(codes);
}

void Shutdown() {
    State& state = GetState();
    if (state.initialized) {
        std::cout << "[input] cerrado.\n";
    }
    state = State{};
}

} // namespace simpsons::input
