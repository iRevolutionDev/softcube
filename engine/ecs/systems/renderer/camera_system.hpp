#pragma once

#include "core/common.hpp"
#include "core/logging.hpp"
#include "core/window.hpp"
#include "ecs/components/renderer/camera_component.hpp"
#include "ecs/components/renderer/camera_controller_component.hpp"
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/systems/system_base.hpp"
#include "input/input_manager.hpp"

namespace softcube::system {
    class CameraSystem final : public System {
        SC_LOG_GROUP(ECS::CAMERA_SYSTEM);

    public:
        CameraSystem(InputManager *input_manager, Window *window) {
            m_input_manager = input_manager;
            m_window = window;
        }

        void init(entt::registry &registry) override {
            System::init(registry);
            m_registry->on_construct<component::Camera>().connect<&CameraSystem::on_camera_construct>(this);
            m_registry->on_destroy<component::Camera>().connect<&CameraSystem::on_camera_destroy>(this);
        }

        /**
         * @brief Gets the main camera entity
         * @return Entity ID of the main camera, or entt::null if not found
         */
        entt::entity get_main_camera() const {
            for (const auto view = m_registry->view<component::Camera>(); const auto entity: view) {
                if (const auto &camera = view.get<component::Camera>(entity); camera.is_main) {
                    return entity;
                }
            }
            return entt::null;
        }

        /**
         * @brief Creates a new main camera entity with controller
         * @param position Initial camera position
         * @param target Point the camera should look at
         * @param use_controller Whether to add a controller component
         * @return Entity ID of the created camera
         */
        entt::entity create_main_camera(const Vector3 &position, const Vector3 &target,
                                        const bool use_controller = true) {
            if (auto existing = get_main_camera(); existing != entt::null) {
                SC_WARN("A main camera already exists. Setting it to non-main.");
                auto &existing_camera = m_registry->get<component::Camera>(existing);
                existing_camera.is_main = false;
            }

            const auto entity = m_registry->create();

            const auto &transform = m_registry->emplace<component::Transform>(entity, position);

            Vector3 direction = target - position;
            direction.normalize();

            Vector3 forward(0.0f, 0.0f, 1.0f);
            const Vector3 up(0.0f, 1.0f, 0.0f);

            if (direction.is_zero()) {
                transform.rotation.identity();
            } else {
                Vector3 right = up.cross(direction);
                right.normalize();

                Vector3 actual_up = direction.cross(right);
                actual_up.normalize();

                Matrix3 rotation_matrix;
                rotation_matrix.set_column(0, right);
                rotation_matrix.set_column(1, actual_up);
                rotation_matrix.set_column(2, direction);

                transform.rotation.from_rotation_matrix(rotation_matrix);
            }

            auto &camera = m_registry->emplace<component::Camera>(entity);
            camera.is_main = true;

            camera.calculate_view_matrix(transform.position, transform.rotation);
            camera.calculate_projection_matrix(static_cast<float>(m_window->get_width()),
                                               static_cast<float>(m_window->get_height()));

            if (use_controller) {
                add_controller(entity);
            }

            SC_INFO("Created main camera entity");
            return entity;
        }

        void update(float dt) override {
            const auto controller_view = m_registry->view<component::Camera, component::Transform,
                component::CameraController>();
            for (const auto entity: controller_view) {
                auto &camera = controller_view.get<component::Camera>(entity);
                auto &transform = controller_view.get<component::Transform>(entity);

                if (auto &controller = controller_view.get<component::CameraController>(entity); controller.is_active) {
                    update_camera_controller(dt, entity, camera, transform, controller);
                }

                camera.calculate_view_matrix(transform.position, transform.rotation);
                camera.calculate_projection_matrix(static_cast<float>(m_window->get_width()),
                                                   static_cast<float>(m_window->get_height()));
            }

            const auto camera_view = m_registry->view<component::Camera, component::Transform>(
                entt::exclude<component::CameraController>);
            for (const auto entity: camera_view) {
                auto &camera = camera_view.get<component::Camera>(entity);
                auto &transform = camera_view.get<component::Transform>(entity);

                camera.calculate_view_matrix(transform.position, transform.rotation);
                camera.calculate_projection_matrix(static_cast<float>(m_window->get_width()),
                                                   static_cast<float>(m_window->get_height()));
            }
        }

        void on_camera_construct(entt::registry &registry, const entt::entity entity) {
            if (const auto &camera = registry.get<component::Camera>(entity); camera.is_main) {
                SC_INFO("Camera is set as main camera");
            }
        }

        void on_camera_destroy(entt::registry &registry, const entt::entity entity) {
            if (const auto &camera = registry.get<component::Camera>(entity); camera.is_main) {
                SC_INFO("Camera is destroyed");
            }
        }

        /**
         * @brief Adds a camera controller to an existing camera entity
         * @param entity The camera entity
         * @param first_person_mode Whether to use first-person mode controls (default: true)
         * @return Reference to the created controller component
         */
        component::CameraController &add_controller(entt::entity entity, bool first_person_mode = true) {
            if (!m_registry->all_of<component::Camera>(entity)) {
                SC_ERROR("Cannot add controller to entity - it does not have a Camera component");
                throw std::runtime_error("Entity does not have a Camera component");
            }

            if (!m_registry->all_of<component::Transform>(entity)) {
                SC_ERROR("Cannot add controller to entity - it does not have a Transform component");
                throw std::runtime_error("Entity does not have a Transform component");
            }

            auto &controller = m_registry->emplace<component::CameraController>(entity);

            SC_INFO("Added camera controller to entity");
            return controller;
        }

        /**
         * @brief Sets the camera controller's movement speed
         * @param entity The camera entity
         * @param speed The movement speed in units per second
         */
        void set_movement_speed(const entt::entity entity, const float speed) {
            if (!m_registry->all_of<component::CameraController>(entity)) {
                SC_WARN("Entity does not have a camera controller");
                return;
            }

            auto &controller = m_registry->get<component::CameraController>(entity);
            controller.movement_speed = speed;
        }

        /**
         * @brief Sets the camera controller's rotation speed
         * @param entity The camera entity
         * @param speed The rotation speed (radians per pixel)
         */
        void set_rotation_speed(const entt::entity entity, const float speed) const {
            if (!m_registry->all_of<component::CameraController>(entity)) {
                SC_WARN("Entity does not have a camera controller");
                return;
            }

            auto &controller = m_registry->get<component::CameraController>(entity);
            controller.rotation_speed = speed;
        }

        /**
         * @brief Sets the camera controller's sensitivity
         * @param entity The camera entity
         * @param sensitivity The sensitivity multiplier
         */
        void set_sensitivity(const entt::entity entity, const float sensitivity) const {
            if (m_registry->all_of<component::CameraController>(entity)) {
                auto &controller = m_registry->get<component::CameraController>(entity);
                controller.sensitivity = sensitivity;
            } else {
                SC_WARN("Entity does not have a camera controller");
            }
        }

    private:
        InputManager *m_input_manager;
        Window *m_window;

        /**
         * @brief Updates camera position and rotation based on input
         * @param dt Delta time in seconds
         * @param entity The camera entity
         * @param camera The camera component
         * @param transform The transform component
         * @param controller The camera controller component
         */
        void update_camera_controller(float dt, entt::entity entity,
                                      component::Camera &camera,
                                      component::Transform &transform,
                                      component::CameraController &controller) const {
            double mouse_x, mouse_y;
            m_input_manager->get_mouse_position(mouse_x, mouse_y);

            if (controller.first_mouse) {
                controller.last_mouse_x = mouse_x;
                controller.last_mouse_y = mouse_y;
                controller.first_mouse = false;
            }

            double offset_x = mouse_x - controller.last_mouse_x;
            double offset_y = controller.last_mouse_y - mouse_y;

            controller.last_mouse_x = mouse_x;
            controller.last_mouse_y = mouse_y;

            offset_x *= controller.rotation_speed * controller.sensitivity;
            offset_y *= controller.rotation_speed * controller.sensitivity;

            bool mouse_right_pressed = m_input_manager->is_mouse_button_pressed(InputManager::MouseButton::Right);

            if (m_input_manager->is_key_pressed(controller.forward_key)) {
                controller.orbit_target += Vector3(0.0f, 0.0f, -1.0f) * controller.movement_speed * dt;
            }
            if (m_input_manager->is_key_pressed(controller.backward_key)) {
                controller.orbit_target += Vector3(0.0f, 0.0f, 1.0f) * controller.movement_speed * dt;
            }
            if (m_input_manager->is_key_pressed(controller.left_key)) {
                controller.orbit_target += Vector3(-1.0f, 0.0f, 0.0f) * controller.movement_speed * dt;
            }
            if (m_input_manager->is_key_pressed(controller.right_key)) {
                controller.orbit_target += Vector3(1.0f, 0.0f, 0.0f) * controller.movement_speed * dt;
            }
            if (m_input_manager->is_key_pressed(controller.up_key)) {
                controller.orbit_target += Vector3(0.0f, 1.0f, 0.0f) * controller.movement_speed * dt;
            }
            if (m_input_manager->is_key_pressed(controller.down_key)) {
                controller.orbit_target += Vector3(0.0f, -1.0f, 0.0f) * controller.movement_speed * dt;
            }

            if (mouse_right_pressed) {
                Vector3 dir = transform.position - controller.orbit_target;
                controller.orbit_distance = dir.length();

                Matrix4 rotation;
                rotation.identity();

                Matrix4 yaw_mat;
                yaw_mat.rotation_y(static_cast<float>(-offset_x));
                rotation = rotation * yaw_mat;

                Matrix4 pitch_mat;
                pitch_mat.rotation_x(static_cast<float>(offset_y));
                rotation = rotation * pitch_mat;

                dir = rotation.transform_vector(dir);

                transform.position = controller.orbit_target + dir;

                Vector3 look_dir = controller.orbit_target - transform.position;
                look_dir.normalize();

                Vector3 up(0.0f, 1.0f, 0.0f);
                Vector3 right = up.cross(look_dir);
                right.normalize();

                Vector3 actual_up = look_dir.cross(right);
                actual_up.normalize();

                Matrix3 rot_matrix;
                rot_matrix.set_column(0, right);
                rot_matrix.set_column(1, actual_up);
                rot_matrix.set_column(2, look_dir);

                transform.rotation.from_rotation_matrix(rot_matrix);
            }

            double scroll_x, scroll_y;
            m_input_manager->get_mouse_scroll(scroll_x, scroll_y);
        }
    };
}
