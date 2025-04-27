#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct MeshRenderer
     * @brief Component for rendering mesh data
     */
    struct MeshRenderer {
        std::vector<bgfx::VertexBufferHandle> vertex_buffers;
        bgfx::IndexBufferHandle index_buffer{BGFX_INVALID_HANDLE};
        bgfx::ProgramHandle shader_program{BGFX_INVALID_HANDLE};

        Vector4 color{1.0f, 1.0f, 1.0f, 1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;

        bgfx::TextureHandle albedo_texture{BGFX_INVALID_HANDLE};
        bgfx::TextureHandle normal_texture{BGFX_INVALID_HANDLE};
        bgfx::TextureHandle metallic_roughness_texture{BGFX_INVALID_HANDLE};

        bool cast_shadows = true;
        bool receive_shadows = true;
        bool visible = true;

        MeshRenderer() = default;

        ~MeshRenderer() {
            for (const auto &vb: vertex_buffers) {
                if (isValid(vb)) {
                    destroy(vb);
                }
            }

            if (isValid(index_buffer)) {
                destroy(index_buffer);
            }
        }
    };
}