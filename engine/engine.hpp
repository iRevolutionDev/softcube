#pragma once
#include "core/common.hpp"
#include "core/logging.hpp"
#include "graphics/renderer/renderer.hpp"

namespace softcube {
    class SceneManager;
    class Renderer;
    class Window;
    class InputManager;
    class EcsManager;

    /**
     * @class Engine
     * @brief The core engine class that manages all subsystems and the game loop.
     *
     * This class serves as the central hub for the game engine, managing all the
     * subsystems and orchestrating the main game loop.
     */
    class Engine {
        SC_LOG_GROUP(ENGINE);

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
        bool run() const;

        /**
         * @brief Shuts down the engine and all subsystems
         */
        void shutdown();

        /**
         * @brief Gets the registry used for the ECS
         * @return Reference to the EnTT registry
         */
        entt::registry &get_registry() { return registry; }

        /**
         * @brief Gets the scene manager
         * @return Pointer to the scene manager
         */
        SceneManager *get_scene_manager() const { return scene_manager.get(); }

        /**
         * @brief Gets the renderer
         * @return Pointer to the renderer
         */
        Renderer *get_renderer() const { return renderer.get(); }

        /**
         * @brief Gets the input manager
         * @return Pointer to the input manager
         */
        InputManager *get_input_manager() const { return input_manager.get(); }

        /**
         * @brief Gets the window
         * @return Pointer to the window
         */
        Window *get_window() const { return window.get(); } /**
         * @brief Gets the ECS manager
         * @return Pointer to the ECS manager
         */
        EcsManager *get_ecs_manager() const { return ecs_manager.get(); }

        /**
         * @brief Toggle the editor mode on/off
         * @param enabled Whether the editor should be enabled
         */
        void set_editor_mode(const bool enabled) const {
            if (!renderer) return;

            renderer->set_editor_enabled(enabled);
        }

        /**
         * @brief Check if the editor mode is enabled
         * @return True if the editor is enabled, false otherwise
         */
        [[nodiscard]] bool is_editor_mode() const {
            return renderer && renderer->is_editor_enabled();
        }

    private:
        std::unique_ptr<Window> window;
        std::unique_ptr<InputManager> input_manager;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<SceneManager> scene_manager;
        std::unique_ptr<EcsManager> ecs_manager;

        entt::registry registry;
        bool is_running;
    };
}
