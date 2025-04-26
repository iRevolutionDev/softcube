#pragma once
#include "core/common.hpp"

namespace softcube {
    class Renderer;

    /**
     * @class Scene
     * @brief Base class for all scenes in the game.
     *
     * Scenes represent different parts of the game, such as menus, game levels, etc.
     * This class provides a common interface for scenes to be loaded, updated, and rendered.
     */
    class Scene {
    public:
        explicit Scene(const std::string &name);

        virtual ~Scene();

        /**
         * @brief Called when the scene is initially loaded
         */
        virtual void on_load() {
        }

        /**
         * @brief Called when the scene becomes the active scene
         */
        virtual void on_enter() {
        }

        /**
         * @brief Called when the scene is no longer the active scene
         */
        virtual void on_leave() {
        }

        /**
         * @brief Called when the scene is unloaded
         */
        virtual void on_unload() {
        }

        /**
         * @brief Updates the scene
         * @param delta_time Time since the last update in seconds
         */
        virtual void update(double delta_time) {
        }

        /**
         * @brief Renders the scene
         * @param renderer Pointer to the renderer
         */
        virtual void render(Renderer *renderer) {
        }

        /**
         * @brief Gets the scene name
         * @return The scene name
         */
        const std::string &get_name() const { return name; }

        /**
         * @brief Gets the scene's entity registry
         * @return Reference to the entity registry
         */
        entt::registry &get_registry() { return registry; }

    protected:
        std::string name;
        entt::registry registry;
    };
}
