#pragma once
#include "engine/scene/scene.hpp"
#include "engine/core/logging.hpp"

namespace game {
    /**
     * @class GameScene
     * @brief Main gameplay scene for the Minecraft-like game.
     *
     * This scene manages the game world, entities, and player interaction.
     */
    class GameScene final : public Scene {
        SC_LOG_GROUP(GAME::SCENE);

    public:
        GameScene();

        virtual ~GameScene();

        void on_load() override;

        void on_enter() override;

        void on_leave() override;

        void on_unload() override;

        void update(double delta_time) override;

        void render(Renderer *renderer) override;
    };
}
