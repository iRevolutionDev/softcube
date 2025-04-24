#include "common.hpp"
#include "engine.hpp"

#include "core/input_manager.hpp"
#include "core/renderer.hpp"
#include "core/window.hpp"
#include "scene/game_scene.hpp"
#include "scene/scene_manager.hpp"

Engine::Engine()
    : is_running(false) {
}

Engine::~Engine() {
    shutdown();
}

bool Engine::init(int argc, char **argv) {
    SC_LOG_INFO("Initializing engine...");

    window = std::make_unique<Window>();
    input_manager = std::make_unique<InputManager>();
    renderer = std::make_unique<Renderer>();
    scene_manager = std::make_unique<SceneManager>();

    if (!window->init(1280, 720, "SoftCube Engine", false)) {
        SC_LOG_ERROR("Failed to initialize window.");
        return false;
    }

    if (!input_manager->init(window.get())) {
        SC_LOG_ERROR("Failed to initialize input manager.");
        return false;
    }

    if (!renderer->init(window.get(), false)) {
        SC_LOG_ERROR("Failed to initialize renderer.");
        return false;
    }

    if (!scene_manager->init()) {
       SC_LOG_ERROR("Failed to initialize scene manager.");
        return false;
    }

    const auto game_scene = std::make_shared<GameScene>();
    scene_manager->add_scene(game_scene);

    is_running = true;

    return true;
}

bool Engine::run() {
    if (!is_running) {
        return false;
    }

    const auto current_time = std::chrono::high_resolution_clock::now();
    delta_time = std::chrono::duration<double>(current_time - last_time).count();
    last_time = current_time;

    input_manager->update(delta_time);

    if (window->get_should_close()) {
        is_running = false;
        return false;
    }

    scene_manager->update(delta_time);

    renderer->begin_frame();

    scene_manager->render(renderer.get());

    renderer->end_frame();

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



