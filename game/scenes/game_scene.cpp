#include "game_scene.hpp"

#include <ecs/components/renderer/camera_controller_component.hpp>

#include "ecs/components/basic/name_component.hpp"
#include "ecs/components/basic/tag_component.hpp"
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/components/renderer/camera_component.hpp"
#include "graphics/renderer/renderer.hpp"
#include "engine/ecs/entity_factory.hpp"
#include "engine/ecs/ecs_manager.hpp"
#include "engine/engine.hpp"
#include "input/input_manager.hpp"

namespace game {
    GameScene::GameScene()
        : Scene("GameScene"), m_engine(nullptr), m_ecs_manager(nullptr) {
    }

    GameScene::~GameScene() = default;

    void GameScene::on_load() {
        SC_INFO("Loading Game Scene");
        m_engine = get_engine();

        if (!m_engine) {
            SC_ERROR("Failed to get engine");
            return;
        }

        m_ecs_manager = m_engine->get_ecs_manager();

        if (!m_ecs_manager) {
            SC_ERROR("Failed to get ECS manager");
            return;
        }

        m_entity_factory = std::make_unique<EntityFactory>(&m_engine->get_registry());

        create_world_entities();
    }

    void GameScene::on_enter() {
        SC_INFO("Entering Game Scene");
    }

    void GameScene::on_leave() {
        SC_INFO("Leaving Game Scene");
    }

    void GameScene::on_unload() {
        SC_INFO("Unloading Game Scene");
    }

    void GameScene::update(double delta_time) {
        if (!m_engine || !m_engine->get_input_manager()) return;

        if (const auto *input = m_engine->get_input_manager(); !input->is_scancode_pressed(SDL_SCANCODE_F12)) return;

        m_editor_mode = !m_editor_mode;
        m_engine->set_editor_mode(m_editor_mode);
        SC_INFO("Editor mode: {}", m_editor_mode ? "enabled" : "disabled");
    }

    void GameScene::render(Renderer *renderer) {
    }

    void GameScene::create_world_entities() {
        if (!m_entity_factory || !m_ecs_manager) return;

        auto main_camera = m_entity_factory->create_camera(
            {0.0f, 3.0f, -10.0f}, true);
        main_camera.get_component<component::Name>().name = "Main Camera";
        main_camera.add_component<component::CameraController>();
        main_camera.add_component<component::Tag>("MainCamera");

        auto parent = m_ecs_manager->create_entity("Parent Object");
        parent.add_component<component::Tag>("ParentTag");
        parent.get_component<component::Transform>().position = {0.0f, 0.0f, 0.0f};

        cube_object = m_entity_factory->create_cube({0.0f, 0.0f, 0.0f}, 1.0f, {0.2f, 0.6f, 1.0f, 1.0f});
        cube_object.get_component<component::Name>().name = "Blue Cube";
        cube_object.add_component<component::Tag>("Cube");
        m_ecs_manager->set_parent(cube_object, parent);

        auto child1 = m_entity_factory->create_cube({1.5f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.2f, 0.2f, 1.0f});
        child1.get_component<component::Name>().name = "Red Cube";
        child1.add_component<component::Tag>("RedCube");
        m_ecs_manager->set_parent(child1, parent);

        auto child2 = m_entity_factory->create_cube({-1.5f, 0.0f, 0.0f}, 0.5f, {0.2f, 1.0f, 0.2f, 1.0f});
        child2.get_component<component::Name>().name = "Green Cube";
        child2.add_component<component::Tag>("GreenCube");
        m_ecs_manager->set_parent(child2, parent);

        auto nested_child = m_entity_factory->create_cube({0.0f, 1.0f, 0.0f}, 0.3f, {1.0f, 1.0f, 0.2f, 1.0f});
        nested_child.get_component<component::Name>().name = "Yellow Cube";
        nested_child.add_component<component::Tag>("YellowCube");
        m_ecs_manager->set_parent(nested_child, child1);

        set_active_camera(main_camera);

        SC_INFO("Created demo scene with camera controllers and entity hierarchy");
    }

    void GameScene::set_active_camera(Entity camera_entity) const {
        for (const auto view = m_engine->get_registry().view<component::Camera>(); const auto entity: view) {
            auto &camera = view.get<component::Camera>(entity);
            camera.is_main = false;
        }

        if (!camera_entity || !camera_entity.has_component<component::Camera>()) return;

        auto &camera = camera_entity.get_component<component::Camera>();
        camera.is_main = true;
        SC_INFO("Activated camera: {}",
                camera_entity.has_component<component::Name>() ?
                camera_entity.get_component<component::Name>().name.c_str() : "Unnamed");

        if (!m_ecs_manager) return;

        m_ecs_manager->set_active_camera(camera_entity);
    }
}
