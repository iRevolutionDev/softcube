#include "engine.hpp"

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

    if (!window->init(1280, 720, "SoftCube Engine", false)) {
        std::cerr << "Failed to initialize window." << std::endl;
        return false;
    }

    is_running = true;

    return true;
}

bool Engine::run() {
    if (!is_running) {
        return false;
    }

    window->update();

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



