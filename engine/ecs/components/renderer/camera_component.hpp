#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Camera
     * @brief Camera component for rendering
     */
    struct Camera {
        float fov = 60.0f;
        float near_clip = 0.1f;
        float far_clip = 1000.0f;
        float aspect_ratio = 16.0f / 9.0f;

        bool is_main = false;
        bool is_orthographic = false;
        float ortho_size = 10.0f;

        Matrix4 view_matrix;
        Matrix4 projection_matrix;

        Camera() = default;

        /**
         * @brief Calculate the view matrix based on transform
         * @param position The position of the entity
         * @param rotation The rotation of the entity
         */
        void calculate_view_matrix(const Vector3 &position, const Quaternion &rotation) const {
            Vector3 forward{0.0f, 0.0f, 1.0f};
            forward = forward * rotation;

            Vector3 up{0.0f, 1.0f, 0.0f};
            up = up * rotation;

            const auto target = position + forward;

            view_matrix.look_at(position, target, up);
        }

        /**
         * @brief Calculate the projection matrix
         * @param width Window width
         * @param height Window height
         */
        void calculate_projection_matrix(const float width, const float height) {
            aspect_ratio = width / height;

            projection_matrix = is_orthographic
                                    ? projection_matrix.orthographic(-ortho_size * aspect_ratio,
                                                                     ortho_size * aspect_ratio,
                                                                     -ortho_size, ortho_size, near_clip, far_clip)
                                    : projection_matrix.perspective(fov, aspect_ratio, near_clip, far_clip);
        }
    };
}
