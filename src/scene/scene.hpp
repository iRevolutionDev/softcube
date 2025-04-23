#pragma once
#include <entt/entity/registry.hpp>

class Renderer;

class Scene {
public:
    Scene(const std::string &name);

    virtual ~Scene();

    virtual void on_load() {
    }

    virtual void on_enter() {
    }

    virtual void on_leave() {
    }

    virtual void on_unload() {
    }

    virtual void update(double deltaTime) {
    }

    virtual void render(Renderer *renderer) {
    }

    const std::string &get_name() const { return name; }

    entt::registry &get_registry() { return registry; }

protected:
    std::string name;
    entt::registry registry;
};
