#include "transform_system.hpp"
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/components/hierarchy/parent_component.hpp"

namespace softcube::system {
    void TransformSystem::update(float dt) {
        for (const auto view = m_registry->view<component::Transform>(); const auto entity: view) {
            auto &transform = view.get<component::Transform>(entity);

            if (m_registry->all_of<component::Parent>(entity)) {
                continue;
            }

            if (transform.parent != entt::null && m_registry->valid(transform.parent)) {
                if (m_registry->all_of<component::Transform>(transform.parent)) {
                    auto &parent_transform = m_registry->get<component::Transform>(transform.parent);

                    transform.position = transform.local_position;
                    transform.rotation = transform.local_rotation;
                    transform.scale = transform.local_scale;

                    Vector3 rotated_pos = transform.position * parent_transform.rotation;

                    rotated_pos.x *= parent_transform.scale.x;
                    rotated_pos.y *= parent_transform.scale.y;
                    rotated_pos.z *= parent_transform.scale.z;

                    transform.position = rotated_pos + parent_transform.position;

                    transform.rotation = parent_transform.rotation * transform.rotation;

                    transform.scale.x *= parent_transform.scale.x;
                    transform.scale.y *= parent_transform.scale.y;
                    transform.scale.z *= parent_transform.scale.z;

                    transform.matrix_dirty = true;
                }
            } else if (transform.parent == entt::null) {
                if (transform.position.x != transform.local_position.x ||
                    transform.position.y != transform.local_position.y ||
                    transform.position.z != transform.local_position.z ||
                    transform.rotation.x != transform.local_rotation.x ||
                    transform.rotation.y != transform.local_rotation.y ||
                    transform.rotation.z != transform.local_rotation.z ||
                    transform.rotation.w != transform.local_rotation.w ||
                    transform.scale.x != transform.local_scale.x ||
                    transform.scale.y != transform.local_scale.y ||
                    transform.scale.z != transform.local_scale.z) {
                    transform.local_position = transform.position;
                    transform.local_rotation = transform.rotation;
                    transform.local_scale = transform.scale;
                    transform.matrix_dirty = true;
                }
            }

            if (transform.matrix_dirty) {
                transform.matrix_dirty = false;
            }
        }
    }
}
