#include "scene.hpp"
#include "engine/engine.hpp"

namespace softcube {
    Scene::Scene(const std::string &name)
        : name(name) {
    }

    Scene::~Scene() = default;

    Engine *Scene::get_engine() {
        extern Engine *g_engine;
        return g_engine;
    }
}
