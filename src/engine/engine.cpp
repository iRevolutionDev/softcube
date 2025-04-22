#include "common.hpp"
#include "engine.hpp"

#include "core/input_manager.hpp"
#include "core/window.hpp"

Engine::Engine()
    : is_running(false) {
}

Engine::~Engine() {
    shutdown();
}

bool Engine::init(int argc, char **argv) {
    std::cout << "Initializing engine..." << std::endl;

    window = std::make_unique<Window>();
    input_manager = std::make_unique<InputManager>();

    if (!window->init(1280, 720, "SoftCube Engine", false)) {
        std::cerr << "Failed to initialize window." << std::endl;
        return false;
    }

    if (!input_manager->init(window.get())) {
        std::cerr << "Failed to initialize input manager." << std::endl;
        return false;
    }

    is_running = true;

    return true;
}

bool Engine::run() {
    if (!is_running) {
        return false;
    }

    auto current_time = std::chrono::high_resolution_clock::now();
    delta_time = std::chrono::duration<double>(current_time - last_time).count();
    last_time = current_time;

    window->update();
    input_manager->update(delta_time);

    if (window->get_should_close()) {
        is_running = false;
        return false;
    }

    return true;
}

void Engine::shutdown() {
    if (!is_running) {
        return;
    }

    is_running = false;

    window.reset();

    std::cout << "Engine shut down." << std::endl;
}



