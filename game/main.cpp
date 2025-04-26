#include "engine/core/common.hpp"
#include "engine/engine.hpp"
#include "scene/scene_manager.hpp"
#include "scenes/game_scene.hpp"

#ifdef SOFTCUBE_PLATFORM_WINDOWS
#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#ifdef SOFTCUBE_DEBUG
    AllocConsole();
#endif

    const auto engine = std::make_unique<Engine>();

    if (!engine->init(__argc, __argv)) {
        return 1;
    }

    const auto game_scene = std::make_shared<game::GameScene>();
    engine->get_scene_manager()->add_scene(game_scene);

    while (engine->run()) {
    }

    engine->shutdown();

    return 0;
}

#else

int main(int argc, char** argv) {
    auto engine = std::make_unique<Engine>();

    if (!engine->init(argc, argv)) {
        return 1;
    }

    const auto game_scene = std::make_shared<game::GameScene>();
    engine->get_scene_manager()->add_scene(game_scene);

    while (engine->run()) {
    }

    engine->shutdown();

    return 0;
}

#endif
