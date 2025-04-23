#include "common.hpp"
#include "scene_manager.hpp"

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
}

bool SceneManager::init() {
    return true;
}

void SceneManager::add_scene(std::shared_ptr<Scene> scene) {
    if (!scene) {
        std::cerr << "Scene is null" << std::endl;
        return;
    }

    const auto name = scene->get_name();
    if (scenes.contains(name)) {
        std::cerr << "Scene already exists" << std::endl;
        return;
    }

    scenes[name] = scene;
    std::cout << "Scene added: " << name.c_str() << std::endl;

    if (!current_scene) {
        scene->on_load();
        scene->on_enter();
        current_scene = scene;

        std::cout << "Current scene: " << name.c_str() << std::endl;
    }
}

void SceneManager::remove_scene(const std::string &name) {
    const auto it = scenes.find(name);

    if (it == scenes.end()) {
        return;
    }

    if (current_scene && current_scene->get_name() == name) {
        std::cerr << "Cannot remove current scene" << std::endl;
        return;
    }

    it->second->on_unload();
    scenes.erase(it);
    std::cout << "Scene removed: " << name << std::endl;
}

bool SceneManager::switch_scene(std::string &name) {
    auto it = scenes.find(name);
    if (it == scenes.end()) {
        std::cerr << "Scene not found: " << name << std::endl;
        return false;
    }

    if (current_scene && current_scene->get_name() == name) {
        std::cerr << "Scene already active: " << name << std::endl;
        return false;
    }

    next_scene = it->second;
    transitioning = true;
    std::cout << "Switching to scene: " << name << std::endl;

    return true;
}

void SceneManager::update(float deltaTime) {
    if (transitioning && next_scene) {
        if (current_scene) {
            current_scene->on_leave();
        }

        next_scene->on_load();
        next_scene->on_enter();

        current_scene = next_scene;
        next_scene = nullptr;
        transitioning = false;
    }

    if (current_scene) {
        current_scene->update(deltaTime);
    }
}

void SceneManager::render(Renderer *renderer) {
    if (!current_scene) {
        return;
    }

    current_scene->render(renderer);
}

Scene *SceneManager::get_current_scene() const {
    return current_scene.get();
}

bool SceneManager::has_scene(const std::string &name) const {
    return scenes.contains(name);
}
