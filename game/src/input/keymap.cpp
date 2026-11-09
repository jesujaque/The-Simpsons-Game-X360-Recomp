#include "input/keymap.h"

#include <algorithm>
#include <cctype>
#include <iterator>

namespace simpsons::input {
namespace {

constexpr const char* kActionNames[] = {
    "confirm", "cancel", "pause", "jump", "attack",
    "interact", "move_up", "move_down", "move_left", "move_right",
};

} // namespace

const char* ActionName(Action action) {
    const auto index = static_cast<std::size_t>(action);
    if (index >= std::size(kActionNames)) {
        return "unknown";
    }
    return kActionNames[index];
}

std::optional<Action> ParseAction(std::string_view name) {
    for (std::size_t index = 0; index < std::size(kActionNames); ++index) {
        if (name == kActionNames[index]) {
            return static_cast<Action>(index);
        }
    }
    return std::nullopt;
}

std::string NormalizeCode(std::string_view code) {
    std::string result;
    result.reserve(code.size());
    for (const char c : code) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

void ActionMap::Bind(std::string code, Action action) {
    const std::string normalized = NormalizeCode(code);
    if (normalized.empty()) {
        return;
    }
    auto& actions = bindings_[normalized];
    if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
        actions.push_back(action);
    }
}

bool ActionMap::BindFromName(std::string code, std::string_view action_name) {
    const auto action = ParseAction(action_name);
    if (!action) {
        return false;
    }
    Bind(std::move(code), *action);
    return true;
}

std::vector<Action> ActionMap::ActionsFor(std::string_view code) const {
    const auto it = bindings_.find(NormalizeCode(code));
    if (it == bindings_.end()) {
        return {};
    }
    return it->second;
}

std::set<Action> ActionMap::Pressed(std::span<const std::string> codes) const {
    std::set<Action> pressed;
    for (const auto& code : codes) {
        for (const Action action : ActionsFor(code)) {
            pressed.insert(action);
        }
    }
    return pressed;
}

std::size_t ActionMap::Size() const {
    std::size_t total = 0;
    for (const auto& [code, actions] : bindings_) {
        (void)code;
        total += actions.size();
    }
    return total;
}

ActionMap ActionMap::Defaults() {
    ActionMap map;
    map.Bind("gamepad_a", Action::Confirm);
    map.Bind("gamepad_b", Action::Cancel);
    map.Bind("gamepad_start", Action::Pause);
    map.Bind("gamepad_x", Action::Attack);
    map.Bind("gamepad_y", Action::Interact);
    map.Bind("gamepad_dpad_up", Action::MoveUp);
    map.Bind("gamepad_dpad_down", Action::MoveDown);
    map.Bind("gamepad_dpad_left", Action::MoveLeft);
    map.Bind("gamepad_dpad_right", Action::MoveRight);

    map.Bind("keyboard_return", Action::Confirm);
    map.Bind("keyboard_space", Action::Jump);
    map.Bind("keyboard_escape", Action::Cancel);
    map.Bind("keyboard_p", Action::Pause);
    map.Bind("keyboard_x", Action::Attack);
    map.Bind("keyboard_e", Action::Interact);
    map.Bind("keyboard_up", Action::MoveUp);
    map.Bind("keyboard_down", Action::MoveDown);
    map.Bind("keyboard_left", Action::MoveLeft);
    map.Bind("keyboard_right", Action::MoveRight);
    return map;
}

} // namespace simpsons::input
