#pragma once
#include <memory>
#include <string>
#include <entt/entt.hpp>

#include "core/logging.hpp"

class Window;
class SceneManager;
class Renderer;
class InputManager;

/**
 * @class Engine
 * @brief The core engine class that manages all subsystems and the game loop.
 * 
 * This class serves as the central hub for the game engine, managing all the 
 * subsystems and orchestrating the main game loop.
 */
class Engine {
    SC_LOG_GROUP(LogGroups::ENGINE);

public:
    Engine();

    ~Engine();

    /**
     * @brief Initializes the engine and all subsystems
     * @param argc Command line argument count
     * @param argv Command line arguments
     * @return True if initialization succeeded, false otherwise
     */
    bool init(int argc, char **argv);

    /**
     * @brief Runs one frame of the engine update cycle
     * @return False if the engine should stop running, true otherwise
     */
    bool run();

    /**
     * @brief Shuts down the engine and all subsystems
     */
    void shutdown();

    /**
     * @brief Gets the registry used for the ECS
     * @return Reference to the EnTT registry
     */
    entt::registry &get_registry() { return registry; }

    // Accessor methods for subsystems
    Window *get_window() const { return window.get(); }
    // Renderer *get_renderer() const { return renderer.get(); }
    // InputManager *get_input_manager() const { return input_manager.get(); }
    // ResourceManager *get_resource_manager() const { return resource_manager.get(); }
    // SceneManager *get_scene_manager() const { return scene_manager.get(); }
    // PhysicsSystem *get_physics_system() const { return physics_system.get(); }
    // AudioSystem *get_audio_system() const { return audio_system.get(); }
    // World *get_world() const { return world.get(); }
    // EventSystem *get_event_system() const { return event_system.get(); }
    // DebugSystem *get_debug_system() const { return debug_system.get(); }

private:
    // Main ECS registry
    entt::registry registry;

    // Core subsystems
    std::unique_ptr<Window> window;
    std::unique_ptr<InputManager> input_manager;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<SceneManager> scene_manager;

    // Engine state
    bool is_running;
    double delta_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time;

    // Configuration
    struct EngineConfig {
        int window_width = 1280;
        int window_height = 720;
        std::string window_title = "SoftCube Engine";
        bool fullscreen = false;
        bool vsync = true;
    } config;
};
