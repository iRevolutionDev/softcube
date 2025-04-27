#include "ecs/ecs_manager.hpp"

#include "ecs/entity.hpp"
#include "components/basic/name_component.hpp"
#include "components/basic/tag_component.hpp"
#include "systems/basic/transform_system.hpp"
#include "systems/hierarchy/hierarchy_system.hpp"
#include "systems/renderer/camera_system.hpp"
#include "systems/renderer/mesh_renderer_system.hpp"

namespace softcube {
    EcsManager::EcsManager() = default;

    EcsManager::~EcsManager() = default;

    void EcsManager::init(entt::registry &registry, Renderer *renderer, InputManager *input_manager, Window *window) {
        m_registry = &registry;
        m_input_manager = input_manager;
        m_window = window;
        m_renderer = renderer;

        m_transform_system = std::make_unique<system::TransformSystem>();
        m_hierarchy_system = std::make_unique<system::HierarchySystem>();
        m_camera_system = std::make_unique<system::CameraSystem>(input_manager, window);
        m_mesh_renderer_system = std::make_unique<system::MeshRendererSystem>(renderer);

        m_transform_system->init(registry);
        m_hierarchy_system->init(registry);
        m_camera_system->init(registry);
        m_mesh_renderer_system->init(registry);

        m_systems.push_back(m_hierarchy_system.get());
        m_systems.push_back(m_transform_system.get());
        m_systems.push_back(m_camera_system.get());
        m_systems.push_back(m_mesh_renderer_system.get());
    }

    void EcsManager::update(const float dt) const {
        for (auto *system: m_systems) {
            if (system->is_enabled()) {
                system->update(dt);
            }
        }
    }

    Entity EcsManager::create_entity(const std::string &name) const {
        const auto entity_handle = m_registry->create();
        Entity entity{entity_handle, m_registry};

        entity.add_component<component::Transform>();

        if (!name.empty()) {
            entity.add_component<component::Name>(name);
        }

        return entity;
    }

    void EcsManager::destroy_entity(const Entity entity) const {
        if (!entity) {
            return;
        }

        m_registry->destroy(entity.get_handle());
    }

    Entity EcsManager::find_entity_by_name(const std::string &name) {
        for (const auto view = m_registry->view<component::Name>(); auto entity: view) {
            if (const auto &name_comp = view.get<component::Name>(entity); name_comp.name == name) {
                return {entity, m_registry};
            }
        }

        return {entt::null, m_registry};
    }

    Entity EcsManager::find_entity_by_tag(const std::string &tag) {
        for (const auto view = m_registry->view<component::Tag>(); auto entity: view) {
            if (const auto &tag_comp = view.get<component::Tag>(entity); tag_comp.tag == tag) {
                return {entity, m_registry};
            }
        }

        return {entt::null, m_registry};
    }

    void EcsManager::set_parent(const Entity child, const Entity parent) const {
        if (m_hierarchy_system) {
            m_hierarchy_system->set_parent(child.get_handle(), parent.get_handle());
        }
    }

    void EcsManager::remove_parent(const Entity child) const {
        if (m_hierarchy_system) {
            m_hierarchy_system->remove_parent(child.get_handle());
        }
    }

    void EcsManager::set_active_camera(const Entity &camera_entity) const {
        if (m_mesh_renderer_system) {
            m_mesh_renderer_system->set_active_camera(camera_entity.get_handle());
        }
    }
}
