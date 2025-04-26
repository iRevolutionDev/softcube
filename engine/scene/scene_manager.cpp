#include "scene_manager.hpp"
#include "graphics/renderer/renderer.hpp"

namespace softcube {
    SceneManager::SceneManager()
        : transitioning(false) {
    }

    SceneManager::~SceneManager() {
        scenes.clear();
        current_scene.reset();
        next_scene.reset();
    }

    bool SceneManager::init() {
        SC_INFO("Initializing scene manager");
        return true;
    }

    void SceneManager::add_scene(const std::shared_ptr<Scene> &scene) {
        const auto &name = scene->get_name();

        if (has_scene(name)) {
            SC_WARN("Scene with name {} already exists, replacing", name);
        }

        scenes[name] = scene;

        if (!current_scene) {
            current_scene = scene;
            current_scene->on_load();
            current_scene->on_enter();
            SC_INFO("Set {} as initial scene", name);
        }
    }

    void SceneManager::remove_scene(const std::string &name) {
        if (!has_scene(name)) {
            SC_WARN("Tried to remove non-existent scene: {}", name);
            return;
        }

        if (current_scene && current_scene->get_name() == name) {
            SC_WARN("Cannot remove the current active scene: {}", name);
            return;
        }

        if (next_scene && next_scene->get_name() == name) {
            SC_WARN("Cannot remove the next scene during transition: {}", name);
            return;
        }

        scenes.erase(name);
        SC_INFO("Removed scene: {}", name);
    }

    bool SceneManager::switch_scene(const std::string &name) {
        if (!has_scene(name)) {
            SC_ERROR("Tried to switch to non-existent scene: {}", name);
            return false;
        }

        if (current_scene && current_scene->get_name() == name) {
            SC_INFO("Already in scene: {}, no switch needed", name);
            return true;
        }

        next_scene = scenes[name];
        transitioning = true;
        SC_INFO("Switching to scene: {}", name);

        return true;
    }

    void SceneManager::update(const float delta_time) {
        if (transitioning && next_scene) {
            if (current_scene) {
                current_scene->on_leave();
            }

            next_scene->on_load();
            next_scene->on_enter();

            current_scene = next_scene;
            next_scene.reset();
            transitioning = false;
        }

        // Update current scene
        if (current_scene) {
            current_scene->update(delta_time);
        }
    }

    void SceneManager::render(Renderer *renderer) const {
        if (!current_scene) {
            return;
        }

        current_scene->render(renderer);
    }

    Scene *SceneManager::get_current_scene() const {
        return current_scene.get();
    }

    bool SceneManager::has_scene(const std::string &name) const {
        return scenes.find(name) != scenes.end();
    }
}
