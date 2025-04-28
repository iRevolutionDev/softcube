#pragma once

#include "core/common.hpp"
#include "core/logging.hpp"
#include "ecs/entity.hpp"
#include "ecs/ecs_manager.hpp"
#include "ecs/components/basic/tag_component.hpp"

namespace softcube {
    namespace component {
        struct Camera;
        struct MeshRenderer;
        struct Transform;
        struct Name;
    }

    /**
     * @class EditorLayer
     * @brief Implements an in-game editor interface for the engine
     *
     * This class provides a visual editor interface with scene hierarchy,
     * entity inspector, and component properties.
     */
    class EditorLayer {
        SC_LOG_GROUP(GRAPHICS::EDITOR_LAYER);

    public:
        EditorLayer();

        ~EditorLayer();

        /**
         * @brief Initialize the editor layer
         * @param ecs_manager Pointer to the ECS manager
         */
        void init(EcsManager *ecs_manager);

        /**
         * @brief Render the editor interface
         */
        void render();

        /**
         * @brief Set the selected entity
         * @param entity The entity to select
         */
        void set_selected_entity(Entity entity);

        /**
         * @brief Get the currently selected entity
         * @return The selected entity
         */
        [[nodiscard]] Entity get_selected_entity() const { return m_selected_entity; }

    private:
        /**
         * @brief Render the scene hierarchy panel
         */
        void render_hierarchy_panel();

        /**
         * @brief Render the inspector panel for the selected entity
         */
        void render_inspector_panel();

        /**
         * @brief Render component properties in the inspector
         * @param entity The entity whose components to render
         */
        void render_components(Entity entity);

        /**
         * @brief Render properties for Transform component
         * @param transform Reference to the Transform component
         */
        void render_transform_component(component::Transform &transform);

        /**
         * @brief Render properties for MeshRenderer component
         * @param mesh_renderer Reference to the MeshRenderer component
         */
        static void render_mesh_renderer_component(component::MeshRenderer &mesh_renderer);

        /**
         * @brief Render properties for Camera component
         * @param camera Reference to the Camera component 
         */
        void render_camera_component(component::Camera &camera) const;

        /**
         * @brief Render properties for Name component
         * @param name Reference to the Name component
         */
        static void render_name_component(component::Name &name);

        /**
         * @brief Render properties for Tag component
         * @param tag Reference to the Tag component
         */
        static void render_tag_component(component::Tag &tag);

        EcsManager *m_ecs_manager = nullptr;
        Entity m_selected_entity;
        bool m_hierarchy_window_open = true;
        bool m_inspector_window_open = true;
        float m_panel_width = 300.0f;
    };
}
