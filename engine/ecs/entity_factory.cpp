#include "entity.hpp"
#include "ecs/entity_factory.hpp"
#include "components/basic/name_component.hpp"
#include "components/basic/transform_component.hpp"
#include "components/renderer/camera_component.hpp"
#include "components/renderer/mesh_renderer_component.hpp"
#include "core/math/math.hpp"
#include "graphics/shaders.hpp"

namespace softcube {
    EntityFactory::EntityFactory(entt::registry *registry)
        : m_registry(registry) {
    }

    Entity EntityFactory::create_camera(const Vector3 &position, const bool is_main) const {
        const entt::entity entity_handle = m_registry->create();
        Entity entity{entity_handle, m_registry};

        entity.add_component<component::Name>("Camera");
        entity.add_component<component::Transform>(position);

        auto &camera = entity.add_component<component::Camera>();
        camera.is_main = is_main;

        return entity;
    }

    Entity EntityFactory::create_cube(const Vector3 &position, const float size, const Vector4 &color) const {
        const entt::entity entity_handle = m_registry->create();
        Entity entity{entity_handle, m_registry};

        entity.add_component<component::Name>("Cube");
        entity.add_component<component::Transform>(position);

        auto &mesh_renderer = entity.add_component<component::MeshRenderer>();
        mesh_renderer.color = color;

        // TODO: Update this to use a mesh component, and load the mesh from a resource manager

        const float half_size = size * 0.5f;

        const Vector3 vertices[8] = {
            {-half_size, -half_size, -half_size}, // 0: left bottom back
            {half_size, -half_size, -half_size}, // 1: right bottom back
            {half_size, half_size, -half_size}, // 2: right top back
            {-half_size, half_size, -half_size}, // 3: left top back
            {-half_size, -half_size, half_size}, // 4: left bottom front
            {half_size, -half_size, half_size}, // 5: right bottom front
            {half_size, half_size, half_size}, // 6: right top front
            {-half_size, half_size, half_size} // 7: left top front
        };

        struct PosNormalVertex {
            float x, y, z;
            float nx, ny, nz;
        };

        const PosNormalVertex cubeVertices[] = {
            // Front face (positive z)
            {vertices[4].x, vertices[4].y, vertices[4].z, 0.0f, 0.0f, 1.0f},
            {vertices[5].x, vertices[5].y, vertices[5].z, 0.0f, 0.0f, 1.0f},
            {vertices[6].x, vertices[6].y, vertices[6].z, 0.0f, 0.0f, 1.0f},
            {vertices[7].x, vertices[7].y, vertices[7].z, 0.0f, 0.0f, 1.0f},

            // Back face (negative z)
            {vertices[1].x, vertices[1].y, vertices[1].z, 0.0f, 0.0f, -1.0f},
            {vertices[0].x, vertices[0].y, vertices[0].z, 0.0f, 0.0f, -1.0f},
            {vertices[3].x, vertices[3].y, vertices[3].z, 0.0f, 0.0f, -1.0f},
            {vertices[2].x, vertices[2].y, vertices[2].z, 0.0f, 0.0f, -1.0f},

            // Right face (positive x)
            {vertices[5].x, vertices[5].y, vertices[5].z, 1.0f, 0.0f, 0.0f},
            {vertices[1].x, vertices[1].y, vertices[1].z, 1.0f, 0.0f, 0.0f},
            {vertices[2].x, vertices[2].y, vertices[2].z, 1.0f, 0.0f, 0.0f},
            {vertices[6].x, vertices[6].y, vertices[6].z, 1.0f, 0.0f, 0.0f},

            // Left face (negative x)
            {vertices[0].x, vertices[0].y, vertices[0].z, -1.0f, 0.0f, 0.0f},
            {vertices[4].x, vertices[4].y, vertices[4].z, -1.0f, 0.0f, 0.0f},
            {vertices[7].x, vertices[7].y, vertices[7].z, -1.0f, 0.0f, 0.0f},
            {vertices[3].x, vertices[3].y, vertices[3].z, -1.0f, 0.0f, 0.0f},

            // Top face (positive y)
            {vertices[7].x, vertices[7].y, vertices[7].z, 0.0f, 1.0f, 0.0f},
            {vertices[6].x, vertices[6].y, vertices[6].z, 0.0f, 1.0f, 0.0f},
            {vertices[2].x, vertices[2].y, vertices[2].z, 0.0f, 1.0f, 0.0f},
            {vertices[3].x, vertices[3].y, vertices[3].z, 0.0f, 1.0f, 0.0f},

            // Bottom face (negative y)
            {vertices[0].x, vertices[0].y, vertices[0].z, 0.0f, -1.0f, 0.0f},
            {vertices[1].x, vertices[1].y, vertices[1].z, 0.0f, -1.0f, 0.0f},
            {vertices[5].x, vertices[5].y, vertices[5].z, 0.0f, -1.0f, 0.0f},
            {vertices[4].x, vertices[4].y, vertices[4].z, 0.0f, -1.0f, 0.0f}
        };

        bgfx::VertexLayout layout;
        layout.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
                .end();

        const bgfx::Memory *vertexMemory = bgfx::copy(cubeVertices, sizeof(cubeVertices));
        const bgfx::VertexBufferHandle vbh = createVertexBuffer(vertexMemory, layout);
        mesh_renderer.vertex_buffers.push_back(vbh);

        const uint16_t indices[] = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Right face
            8, 9, 10, 8, 10, 11,
            // Left face
            12, 13, 14, 12, 14, 15,
            // Top face
            16, 17, 18, 16, 18, 19,
            // Bottom face
            20, 21, 22, 20, 22, 23
        };

        const bgfx::Memory *indexMemory = bgfx::copy(indices, sizeof(indices));
        mesh_renderer.index_buffer = createIndexBuffer(indexMemory);

        mesh_renderer.shader_program = createProgram(
            createEmbeddedShader(&k_simple_vs, bgfx::getRendererType(), "v_simple"),
            createEmbeddedShader(&k_simple_fs, bgfx::getRendererType(), "f_simple"),
            true
        );

        return entity;
    }
}
