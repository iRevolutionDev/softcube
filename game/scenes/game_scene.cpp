#include "game_scene.hpp"

#include <ecs/components/renderer/camera_controller_component.hpp>

#include "ecs/components/basic/name_component.hpp"
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/components/renderer/camera_component.hpp"
#include "graphics/renderer/renderer.hpp"
#include "engine/ecs/entity_factory.hpp"
#include "engine/ecs/ecs_manager.hpp"
#include "engine/engine.hpp"
#include "core/math/math_utils.hpp"
#include "ecs/components/renderer/mesh_renderer_component.hpp"

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
    }

    void GameScene::render(Renderer *renderer) {
        ImGui::Begin("Game Scene");

        ImGui::Text("Camera Controls:");
        ImGui::BulletText("Right mouse button + Mouse move: Look around");
        ImGui::BulletText("WASD: Move camera");
        ImGui::BulletText("Q/E: Move up/down");
        ImGui::BulletText("Shift: Run");
        ImGui::BulletText("Mouse wheel: Zoom (orbit camera)");

        if (ImGui::Button("Switch to FPS Camera")) {
            set_active_camera(m_fps_camera);
        }

        ImGui::SameLine();

        ImGui::Separator();
        ImGui::Text("Test Cube");
        static float cube_color[4] = {0.2f, 0.6f, 1.0f, 1.0f};
        if (ImGui::ColorEdit4("Cube Color", cube_color)) {
            if (cube_object && cube_object.has_component<component::MeshRenderer>()) {
                auto &mesh_renderer = cube_object.get_component<component::MeshRenderer>();
                mesh_renderer.color = Vector4(cube_color[0], cube_color[1], cube_color[2], cube_color[3]);
            }
        }

        ImGui::End();

        update_camera_debug_ui();
    }

    void GameScene::create_world_entities() const {
        if (!m_entity_factory || !m_ecs_manager) return;

        auto main_camera = m_entity_factory->create_camera(
            {0.0f, 1.0f, -5.0f}, true);

        main_camera.add_component<component::CameraController>();

        set_active_camera(main_camera);

        SC_INFO("Created demo scene with camera controllers and entity hierarchy");
    }

    void GameScene::set_active_camera(Entity camera_entity) const {
        for (const auto view = m_engine->get_registry().view<component::Camera>(); auto entity: view) {
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

    void GameScene::update_camera_debug_ui() {
        if (cube_object) {
            auto &transform = cube_object.get_component<component::Transform>();

            const auto rotation = Quaternion::rotation_y(0.01f);
            transform.rotation = rotation * transform.rotation;
            transform.local_rotation = transform.rotation;
        }

        if (ImGui::Begin("Camera Debug")) {
            for (const auto view = m_engine->get_registry().view<component::Transform, component::Camera>(); const auto
                 entity
                 :
                 view) {
                const auto &transform = view.get<component::Transform>(entity);
                const auto &camera = view.get<component::Camera>(entity);

                std::string name = "Camera";
                if (const Entity camera_entity{entity, &m_engine->get_registry()}; camera_entity.has_component<
                    component::Name>()) {
                    name = camera_entity.get_component<component::Name>().name;
                }

                if (ImGui::TreeNode(name.c_str())) {
                    ImGui::Text("Position: [%.2f, %.2f, %.2f]",
                                transform.position.x, transform.position.y, transform.position.z);

                    ImGui::Text("Active: %s", camera.is_main ? "Yes" : "No");

                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }
    }
}
