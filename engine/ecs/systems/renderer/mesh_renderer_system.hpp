#pragma once

#include "core/common.hpp"
#include "core/logging.hpp"
#include "ecs/components/renderer/mesh_renderer_component.hpp"
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/systems/system_base.hpp"
#include "graphics/renderer/renderer.hpp"

namespace softcube::system {
    /**
     * @class MeshRendererSystem
     * @brief System for rendering 3D meshes
     * 
     * This system handles rendering of entities with MeshRenderer components.
     * It processes all entities with MeshRenderer and Transform components and
     * submits them to the renderer for drawing.
     */
    class MeshRendererSystem final : public System {
        SC_LOG_GROUP(ECS::MESH_RENDERER_SYSTEM);

    public:
        explicit MeshRendererSystem(Renderer *renderer);

        /**
         * @brief Initialize the system
         * @param registry Reference to the EnTT registry
         */
        void init(entt::registry &registry) override;

        /**
         * @brief Update the system
         * @param dt Delta time in seconds
         */
        void update(float dt) override;

        /**
         * @brief Set the active camera for rendering
         * @param camera_entity Entity ID of the camera to use
         */
        void set_active_camera(entt::entity camera_entity);

    private:
        Renderer *m_renderer = nullptr;
        entt::entity m_active_camera = entt::null;

        void on_mesh_renderer_construct(entt::registry &registry, entt::entity entity);

        void on_mesh_renderer_destroy(entt::registry &registry, entt::entity entity);

        void submit_mesh(entt::entity entity,
                         const component::Transform &transform,
                         const component::MeshRenderer &mesh_renderer);
    };
}
