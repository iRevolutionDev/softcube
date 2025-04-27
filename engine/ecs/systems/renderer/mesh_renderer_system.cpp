#include "mesh_renderer_system.hpp"

#include "ecs/components/basic/name_component.hpp"
#include "ecs/components/renderer/camera_component.hpp"

namespace softcube::system {
    MeshRendererSystem::MeshRendererSystem(Renderer *renderer)
        : m_renderer(renderer) {
    }

    void MeshRendererSystem::init(entt::registry &registry) {
        System::init(registry);

        m_registry->on_construct<component::MeshRenderer>()
                .connect<&MeshRendererSystem::on_mesh_renderer_construct>(this);

        m_registry->on_destroy<component::MeshRenderer>()
                .connect<&MeshRendererSystem::on_mesh_renderer_destroy>(this);

        SC_INFO("MeshRendererSystem initialized");
    }

    void MeshRendererSystem::update(float dt) {
        if (m_active_camera == entt::null) {
            return;
        }

        if (!m_registry->valid(m_active_camera) ||
            !m_registry->all_of<component::Camera, component::Transform>(m_active_camera)) {
            SC_WARN("Active camera entity is invalid or missing required components");
            m_active_camera = entt::null;
            return;
        }

        const auto &camera = m_registry->get<component::Camera>(m_active_camera);
        const auto &camera_transform = m_registry->get<component::Transform>(m_active_camera);

        camera.calculate_view_matrix(camera_transform.position, camera_transform.rotation);

        bgfx::setViewTransform(
            0,
            camera.view_matrix.values,
            camera.projection_matrix.values
        );

        for (auto view = m_registry->view<component::MeshRenderer, component::Transform>(); const auto entity: view) {
            const auto &mesh_renderer = view.get<component::MeshRenderer>(entity);
            const auto &transform = view.get<component::Transform>(entity);

            if (!mesh_renderer.visible) {
                continue;
            }

            submit_mesh(entity, transform, mesh_renderer);
        }
    }

    void MeshRendererSystem::set_active_camera(entt::entity camera_entity) {
        m_active_camera = camera_entity;

        if (camera_entity != entt::null) {
            SC_INFO("Set active camera: {}",
                    m_registry->all_of<component::Name>(camera_entity) ?
                    m_registry->get<component::Name>(camera_entity).name.c_str() : "Unnamed");
        } else {
            SC_INFO("Active camera cleared");
        }
    }

    void MeshRendererSystem::on_mesh_renderer_construct(entt::registry &registry, entt::entity entity) {
        SC_DEBUG("MeshRenderer component added to entity {}", static_cast<uint32_t>(entity));
    }

    void MeshRendererSystem::on_mesh_renderer_destroy(entt::registry &registry, entt::entity entity) {
        SC_DEBUG("MeshRenderer component removed from entity {}", static_cast<uint32_t>(entity));
    }

    void MeshRendererSystem::submit_mesh(entt::entity entity,
                                         const component::Transform &transform,
                                         const component::MeshRenderer &mesh_renderer) {
        if (mesh_renderer.vertex_buffers.empty() || !isValid(mesh_renderer.index_buffer)) {
            return;
        }

        float model[16];

        if (transform.matrix_dirty) {
            Matrix4 model_matrix = transform.world_matrix;
            std::memcpy(model, model_matrix.values, sizeof(float) * 16);
        } else {
            Matrix4 model_matrix;

            Matrix4 translation_matrix = Matrix4::translation(transform.position);
            Matrix4 rotation_matrix(transform.rotation.to_rotation_matrix());
            Matrix4 scale_matrix = Matrix4::scale(transform.scale);
            model_matrix = translation_matrix * rotation_matrix * scale_matrix;
            std::memcpy(model, model_matrix.values, sizeof(float) * 16);
        }

        bgfx::setTransform(model);

        for (const auto &vb: mesh_renderer.vertex_buffers) {
            setVertexBuffer(0, vb);
        }
        setIndexBuffer(mesh_renderer.index_buffer);


        bgfx::UniformHandle u_color = createUniform("u_color", bgfx::UniformType::Vec4);
        if (isValid(u_color)) {
            float color[4] = {
                mesh_renderer.color.x,
                mesh_renderer.color.y,
                mesh_renderer.color.z,
                mesh_renderer.color.w
            };
            setUniform(u_color, color);
        }

        uint64_t state = 0
                         | BGFX_STATE_WRITE_RGB
                         | BGFX_STATE_WRITE_A
                         | BGFX_STATE_WRITE_Z
                         | BGFX_STATE_DEPTH_TEST_LESS
                         | BGFX_STATE_CULL_CCW
                         | BGFX_STATE_MSAA;

        bgfx::setState(state);
        if (isValid(mesh_renderer.shader_program)) {
            submit(0, mesh_renderer.shader_program);
        }

        // Clean up
        if (isValid(u_color)) {
            destroy(u_color);
        }
    }
}
