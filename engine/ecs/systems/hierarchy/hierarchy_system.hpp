#pragma once
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/components/hierarchy/parent_component.hpp"
#include "ecs/systems/system_base.hpp"

#include "core/logging.hpp"
#include "ecs/components/hierarchy/children_component.hpp"

namespace softcube {
    class InputManager;
    class Window;
}

namespace softcube::system {
    /**
     * @class HierarchySystem
     * @brief System for managing entity hierarchies
     * 
     * This system handles parent-child relationships between entities
     * and ensures proper propagation of transforms through the hierarchy.
     */
    class HierarchySystem final : public System {
        SC_LOG_GROUP(ECS::HierarchySystem);

    public:
        HierarchySystem() = default;

        void init(entt::registry &registry) override {
            System::init(registry);

            m_registry->on_construct<component::Parent>().connect<&HierarchySystem::on_parent_construct>(this);
            m_registry->on_destroy<component::Parent>().connect<&HierarchySystem::on_parent_destroy>(this);
        }

        void update(float dt) override {
            for (const auto view = m_registry->view<component::Transform, component::Parent>(); const auto entity:
                 view) {
                auto &transform = view.get<component::Transform>(entity);

                if (const auto &parent = view.get<component::Parent>(entity);
                    parent.entity != entt::null && m_registry->valid(parent.entity)) {
                    if (m_registry->all_of<component::Transform>(parent.entity)) {
                        auto &parent_transform = m_registry->get<component::Transform>(parent.entity);

                        transform.position = transform.local_position;
                        transform.rotation = transform.local_rotation;
                        transform.scale = transform.local_scale;

                        Vector3 rotated_pos = transform.position * parent_transform.rotation;

                        rotated_pos.x *= parent_transform.scale.x;
                        rotated_pos.y *= parent_transform.scale.y;
                        rotated_pos.z *= parent_transform.scale.z;

                        transform.position = bx::add(rotated_pos, parent_transform.position);

                        transform.rotation = parent_transform.rotation * transform.rotation;

                        transform.scale.x *= parent_transform.scale.x;
                        transform.scale.y *= parent_transform.scale.y;
                        transform.scale.z *= parent_transform.scale.z;

                        transform.matrix_dirty = true;
                    }
                } else {
                    transform.position = transform.local_position;
                    transform.rotation = transform.local_rotation;
                    transform.scale = transform.local_scale;
                    transform.matrix_dirty = true;
                }
            }

            for (const auto transform_view = m_registry->view<component::Transform>(entt::exclude<component::Parent>);
                 const auto entity: transform_view) {
                if (auto &transform = transform_view.get<component::Transform>(entity);
                    transform.parent == entt::null) {
                    transform.position = transform.local_position;
                    transform.rotation = transform.local_rotation;
                    transform.scale = transform.local_scale;
                    transform.matrix_dirty = true;
                }
            }
        }

        /**
         * @brief Set parent-child relationship between entities
         * @param child Child entity
         * @param parent Parent entity
         */
        void set_parent(const entt::entity child, entt::entity parent) {
            if (child == parent) return;

            if (is_ancestor(child, parent)) {
                SC_ERROR("Cannot set parent: Would create a cycle in the hierarchy");
                return;
            }

            if (m_registry->all_of<component::Parent>(child)) {
                remove_from_parent(child);
            }

            m_registry->emplace_or_replace<component::Parent>(child, parent);

            if (m_registry->all_of<component::Transform>(child)) {
                auto &transform = m_registry->get<component::Transform>(child);
                transform.parent = parent;

                if (m_registry->all_of<component::Transform>(parent)) {
                    const auto &parent_transform = m_registry->get<component::Transform>(parent);

                    const Vector3 relative_pos = transform.position - parent_transform.position;
                    const Quaternion inv_rotation = parent_transform.rotation.inverse();

                    transform.local_scale.x = transform.scale.x / parent_transform.scale.x;
                    transform.local_scale.y = transform.scale.y / parent_transform.scale.y;
                    transform.local_scale.z = transform.scale.z / parent_transform.scale.z;

                    transform.local_rotation = inv_rotation * transform.rotation;
                }
            }

            if (!m_registry->all_of<component::Children>(parent)) {
                m_registry->emplace<component::Children>(parent);
            }

            auto &children = m_registry->get<component::Children>(parent);
            children.add_child(child);
        }

        /**
         * @brief Remove parent-child relationship
         * @param child Child entity to detach from its parent
         */
        void remove_parent(const entt::entity child) const {
            if (m_registry->all_of<component::Parent>(child)) {
                remove_from_parent(child);
                m_registry->remove<component::Parent>(child);

                if (m_registry->all_of<component::Transform>(child)) {
                    auto &transform = m_registry->get<component::Transform>(child);
                    transform.local_position = transform.position;
                    transform.local_rotation = transform.rotation;
                    transform.local_scale = transform.scale;
                    transform.parent = entt::null;
                }
            }
        }

        /**
                 * @brief Check if entity is an ancestor of potential_child
                 * @param entity Entity to check
                 * @param potential_child Entity that might be a descendant
                 * @return True if entity is an ancestor of potential_child
                 */
        bool is_ancestor(const entt::entity entity, const entt::entity potential_child) {
            if (entity == potential_child) return true;
            if (potential_child == entt::null) return false;

            if (m_registry->all_of<component::Parent>(potential_child)) {
                const auto &parent = m_registry->get<component::Parent>(potential_child);
                return is_ancestor(entity, parent.entity);
            }

            return false;
        }

    private:
        /**
         * @brief Remove child from its current parent's children list
         * @param child Child entity to remove
         */
        void remove_from_parent(const entt::entity child) const {
            if (!m_registry->all_of<component::Parent>(child)) return;

            const auto &parent_comp = m_registry->get<component::Parent>(child);

            if (const auto parent = parent_comp.entity; parent != entt::null && m_registry->valid(parent)) {
                if (m_registry->all_of<component::Children>(parent)) {
                    auto &children = m_registry->get<component::Children>(parent);
                    children.remove_child(child);

                    if (children.entities.empty()) {
                        m_registry->remove<component::Children>(parent);
                    }
                }
            }
        }

        void on_parent_construct(entt::registry &registry, const entt::entity entity) {
            const auto &parent = registry.get<component::Parent>(entity);

            if (const auto parent_entity = parent.entity;
                parent_entity != entt::null && registry.valid(parent_entity)) {
                if (!registry.all_of<component::Children>(parent_entity)) {
                    registry.emplace<component::Children>(parent_entity);
                }

                auto &children = registry.get<component::Children>(parent_entity);
                children.add_child(entity);

                if (registry.all_of<component::Transform>(entity)) {
                    registry.get<component::Transform>(entity).parent = parent_entity;
                }
            }
        }

        void on_parent_destroy(entt::registry &registry, const entt::entity entity) const {
            remove_from_parent(entity);

            if (registry.all_of<component::Transform>(entity)) {
                registry.get<component::Transform>(entity).parent = entt::null;
            }
        }
    };
}
