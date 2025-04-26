#pragma once
#include "core/common.hpp"
#include "core/logging.hpp"
#include "scene.hpp"

namespace softcube {
    class Renderer;

    /**
     * @class SceneManager
     * @brief Manages scenes and scene transitions.
     *
     * This class is responsible for storing, loading, and transitioning between scenes.
     */
    class SceneManager {
        SC_LOG_GROUP(SCENE::MANAGER);

    public:
        SceneManager();

        ~SceneManager();

        /**
         * @brief Initializes the scene manager
         * @return True if initialization succeeded, false otherwise
         */
        bool init();

        /**
         * @brief Adds a scene to the manager
         * @param scene Shared pointer to the scene to add
         */
        void add_scene(const std::shared_ptr<Scene> &scene);

        /**
         * @brief Removes a scene from the manager
         * @param name The name of the scene to remove
         */
        void remove_scene(const std::string &name);

        /**
         * @brief Switches to a different scene
         * @param name The name of the scene to switch to
         * @return True if the scene was found and the switch was initiated
         */
        bool switch_scene(const std::string &name);

        /**
         * @brief Updates the current scene
         * @param delta_time Time since the last update in seconds
         */
        void update(float delta_time);

        /**
         * @brief Renders the current scene
         * @param renderer Pointer to the renderer
         */
        void render(Renderer *renderer) const;

        /**
         * @brief Gets the current active scene
         * @return Pointer to the current scene
         */
        Scene *get_current_scene() const;

        /**
         * @brief Checks if a scene exists in the manager
         * @param name The name of the scene to check for
         * @return True if the scene exists, false otherwise
         */
        bool has_scene(const std::string &name) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<Scene> > scenes;
        std::shared_ptr<Scene> current_scene;
        std::shared_ptr<Scene> next_scene;
        bool transitioning;
    };
}
