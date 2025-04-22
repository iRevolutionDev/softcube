#include <common.hpp>

#include "engine/engine.hpp"

#ifdef SOFT_CUBE_PLATFORM_WINDOWS

#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    const auto engine_instance = std::make_unique<Engine>();

    engine_instance->init(__argc, __argv);

    while (engine_instance->run()) {
    }

    engine_instance->shutdown();

    return 0;
}

#else

int main(int argc, char **argv) {
    auto engine_instance = std::make_unique<Engine>();

    engine_instance->init(argc, argv);

    while (engine_instance->run()) {
    }

    engine_instance->shutdown();

    return 0;
}

#endif
