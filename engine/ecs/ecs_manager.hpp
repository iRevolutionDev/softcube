#pragma once
#include "core/common.hpp"

namespace softcube {
    class Entity;

    namespace system {
        class CameraSystem;
        class System;
        class HierarchySystem;
        class TransformSystem;
        class MeshRendererSystem;
    }

    class Renderer;
    class InputManager;
    class Window;

    /**
     * @class EcsManager
     * @brief Manager class for the Entity Component System
     *
     * This class is responsible for creating and managing entities,
     * components, and systems. It serves as the main interface to the ECS.
     */
    class EcsManager {
    public:
        EcsManager();

        ~EcsManager();

        /**
       * @brief Initialize the ECS manager
       * @param registry The EnTT registry to use
       * @param renderer Pointer to the renderer
       * @param input_manager Pointer to the input manager
       * @param window Pointer to the window
       */
        void init(entt::registry &registry, Renderer *renderer, InputManager *input_manager = nullptr,
                  Window *window = nullptr);

        /**
         * @brief Update all systems
         * @param dt Delta time in seconds
         */
        void update(float dt) const;

        /**
         * @brief Create a new entity
         * @param name Optional name for the entity
         * @return The created entity
         */
        Entity create_entity(const std::string &name = "Entity") const;

        /**
         * @brief Destroy an entity
         * @param entity The entity to destroy
         */
        void destroy_entity(Entity entity) const;

        /**
         * @brief Find an entity by name
         * @param name The name to search for
         * @return The found entity, or an invalid entity if not found
         */
        Entity find_entity_by_name(const std::string &name);

        /**
         * @brief Find an entity by tag
         * @param tag The tag to search for
         * @return The found entity, or an invalid entity if not found
         */
        Entity find_entity_by_tag(const std::string &tag);

        /**
         * @brief Set the active camera for rendering
         * @param camera_entity Entity with camera component
         */
        void set_active_camera(const Entity &camera_entity) const;

        /**
       * @brief Get the transform system
       * @return Reference to the transform system
       */
        system::TransformSystem &get_transform_system() const { return *m_transform_system; }

        /**
         * @brief Get the hierarchy system
         * @return Reference to the hierarchy system
         */
        system::HierarchySystem &get_hierarchy_system() const { return *m_hierarchy_system; }

        /**
         * @brief Get the mesh renderer system
         * @return Reference to the mesh renderer system
         */
        system::MeshRendererSystem &get_mesh_renderer_system() const { return *m_mesh_renderer_system; }

        /**
         * @brief Set parent-child relationship between entities
         * @param child Child entity
         * @param parent Parent entity
         */
        void set_parent(Entity child, Entity parent) const;

        /**
         * @brief Remove parent-child relationship
         * @param child Child entity to detach
         */
        void remove_parent(Entity child) const;

    private:
        entt::registry *m_registry = nullptr;
        InputManager *m_input_manager = nullptr;
        Window *m_window = nullptr;
        Renderer *m_renderer = nullptr;

        std::unique_ptr<system::TransformSystem> m_transform_system;
        std::unique_ptr<system::HierarchySystem> m_hierarchy_system;
        std::unique_ptr<system::CameraSystem> m_camera_system;
        std::unique_ptr<system::MeshRendererSystem> m_mesh_renderer_system;

        std::vector<system::System *> m_systems;
    };
}
