#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Transform
     * @brief Component to store position, rotation, and scale data
     */
    struct Transform {
        Vector3 position{0.0f, 0.0f, 0.0f};
        Quaternion rotation{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 scale{1.0f, 1.0f, 1.0f};

        Vector3 local_position{0.0f, 0.0f, 0.0f};
        Quaternion local_rotation{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 local_scale{1.0f, 1.0f, 1.0f};

        entt::entity parent = entt::null;

        mutable bool matrix_dirty = true;
        mutable Matrix4 local_matrix;
        mutable Matrix4 world_matrix;

        Transform() = default;

        explicit Transform(const bx::Vec3 &pos) : position(pos), local_position(pos) {
        }

        Transform(const bx::Vec3 &pos, const bx::Quaternion &rot)
            : position(pos), rotation(rot), local_position(pos), local_rotation(rot) {
        }

        Transform(const bx::Vec3 &pos, const bx::Quaternion &rot, const bx::Vec3 &sc)
            : position(pos), rotation(rot), scale(sc), local_position(pos), local_rotation(rot), local_scale(sc) {
        }

        /**
         * @brief Get the forward direction vector
         * @return The forward direction vector
         */
        [[nodiscard]] Vector3 get_forward() const {
            Vector3 forward{0.0f, 0.0f, 1.0f};
            forward = forward * rotation;
            return forward;
        }

        /**
         * @brief Get the right direction vector
         * @return The right direction vector
         */
        [[nodiscard]] Vector3 get_right() const {
            Vector3 right{1.0f, 0.0f, 0.0f};
            right = right * rotation;
            return right;
        }

        /**
         * @brief Get the up direction vector
         * @return The up direction vector
         */
        [[nodiscard]] Vector3 get_up() const {
            Vector3 up{0.0f, 1.0f, 0.0f};
            up = up * rotation;
            return up;
        }
    };
}