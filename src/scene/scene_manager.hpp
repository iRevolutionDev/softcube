#pragma once
#include "core/renderer.hpp"
#include "scene.hpp"
#include "core/logging.hpp"

class SceneManager {
    SC_LOG_GROUP(SCENE_MANAGER);

public:
    SceneManager();

    ~SceneManager();

    bool init();

    void add_scene(std::shared_ptr<Scene> scene);

    void remove_scene(const std::string &name);

    bool switch_scene(std::string &name);

    void update(float deltaTime);

    void render(Renderer *renderer);

    Scene *get_current_scene() const;

    bool has_scene(const std::string &name) const;

private:
    std::unordered_map<std::string, std::shared_ptr<Scene> > scenes;
    std::shared_ptr<Scene> current_scene;
    std::shared_ptr<Scene> next_scene;
    bool transitioning{};
};
