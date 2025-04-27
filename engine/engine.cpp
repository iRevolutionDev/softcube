#include "engine.hpp"
#include "core/window.hpp"
#include "input/input_manager.hpp"
#include "graphics/renderer/renderer.hpp"
#include "scene/scene_manager.hpp"
#include "ecs/ecs_manager.hpp"

using namespace softcube;

Engine::Engine()
    : is_running(false) {
    g_engine = this;
}

Engine::~Engine() {
    shutdown();
    g_engine = nullptr;
}

bool Engine::init(int argc, char **argv) {
    SC_INFO("Initializing engine...");
    window = std::make_unique<Window>();
    input_manager = std::make_unique<InputManager>();
    renderer = std::make_unique<Renderer>();
    scene_manager = std::make_unique<SceneManager>();
    ecs_manager = std::make_unique<EcsManager>();

    if (!window->init(1280, 720, "SoftCube Engine", false)) {
        SC_ERROR("Failed to initialize window");
        return false;
    }

    if (!input_manager->init(window.get())) {
        SC_ERROR("Failed to initialize input manager");
        return false;
    }

    if (!renderer->init(window.get(), false)) {
        SC_ERROR("Failed to initialize renderer");
        return false;
    }
    if (!scene_manager->init()) {
        SC_ERROR("Failed to initialize scene manager");
        return false;
    }

    ecs_manager->init(registry, renderer.get(), input_manager.get(), window.get());
    SC_INFO("ECS manager initialized");

    is_running = true;
    SC_INFO("Engine initialization complete");

    return true;
}

bool Engine::run() const {
    if (!is_running) {
        return false;
    }

    window->update();
    input_manager->update();

    if (window->get_should_close()) {
        return false;
    }
    static auto last_time = std::chrono::high_resolution_clock::now();
    const auto current_time = std::chrono::high_resolution_clock::now();
    const float delta_time = std::chrono::duration<float>(current_time - last_time).count();
    last_time = current_time;

    ecs_manager->update(delta_time);
    scene_manager->update(delta_time);

    renderer->begin_imgui();
    renderer->begin_frame();
    scene_manager->render(renderer.get());
    renderer->end_frame();
    renderer->end_imgui();

    return true;
}

void Engine::shutdown() {
    if (is_running) {
        SC_INFO("Shutting down engine...");

        scene_manager.reset();
        renderer.reset();
        input_manager.reset();
        window.reset();

        is_running = false;
    }
}
