#pragma once
#include "engine/scene/scene.hpp"
#include "engine/core/logging.hpp"
#include "engine/ecs/entity.hpp"

namespace softcube {
    class EntityFactory;
    class EcsManager;
    class Engine;
}

using namespace softcube;

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

        ~GameScene() override;

        void on_load() override;

        void on_enter() override;

        void on_leave() override;

        void on_unload() override;

        void update(double delta_time) override;

        void render(Renderer *renderer) override;

    private:
        /**
         * @brief Create world entities including cameras and objects
         */
        void create_world_entities();

        /**
         * @brief Set the active camera
         * @param camera_entity The camera entity to make active
         */
        void set_active_camera(Entity camera_entity) const;

        Engine *m_engine;
        EcsManager *m_ecs_manager;
        std::unique_ptr<EntityFactory> m_entity_factory;

        Entity m_fps_camera;
        Entity cube_object;
        bool m_editor_mode = true;
    };
}
