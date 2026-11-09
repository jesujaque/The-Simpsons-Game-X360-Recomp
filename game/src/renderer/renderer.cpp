#include "renderer/renderer.h"

#include <chrono>
#include <iostream>

namespace simpsons::renderer {
namespace {

struct State {
    bool initialized = false;
    bool headless = false;
    RendererConfig config;
    FrameStats stats;
    std::chrono::steady_clock::time_point last_present;
};

State& GetState() {
    static State state;
    return state;
}

} // namespace

bool Initialize(const RendererConfig& config) {
    State& state = GetState();
    if (config.width <= 0 || config.height <= 0) {
        std::cerr << "[render] resolución inválida.\n";
        return false;
    }

    state = State{};
    state.config = config;
    state.headless = config.headless;
    state.initialized = true;

    if (state.headless) {
        std::cout << "[render] modo headless " << config.width << "x" << config.height << ".\n";
    } else {
        std::cout << "[render] backend=" << config.backend << " " << config.width << "x"
                  << config.height << " vsync=" << (config.vsync ? "on" : "off")
                  << " max_fps=" << config.max_fps << ".\n";
        std::cout << "[render] ventana real pendiente de enlazar ReXGlue; "
                     "se usa present simulado.\n";
    }
    return true;
}

void Present() {
    State& state = GetState();
    if (!state.initialized) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (state.stats.frame_count > 0) {
        state.stats.last_frame_ms =
            std::chrono::duration<double, std::milli>(now - state.last_present).count();
    }
    state.last_present = now;
    ++state.stats.frame_count;
}

void Shutdown() {
    State& state = GetState();
    if (state.initialized) {
        std::cout << "[render] cerrado tras " << state.stats.frame_count << " frames.\n";
    }
    state = State{};
}

bool IsInitialized() {
    return GetState().initialized;
}

bool IsHeadless() {
    return GetState().headless;
}

FrameStats Stats() {
    return GetState().stats;
}

} // namespace simpsons::renderer
