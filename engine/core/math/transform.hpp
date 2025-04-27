#pragma once
#include "core/common.hpp"
#include "vector3.hpp"
#include "quaternion.hpp"
#include "matrix.hpp"

namespace softcube {
    /**
     * @struct Transform
     * @brief Represents a transformation in 3D space with position, rotation, and scale
     */
    struct Transform {
        Vector3 position{0.0f, 0.0f, 0.0f};
        Quaternion rotation{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 scale{1.0f, 1.0f, 1.0f};

        // Constructors
        Transform() = default;

        Transform(const Vector3 &position)
            : position(position) {
        }

        Transform(const Vector3 &position, const Quaternion &rotation)
            : position(position), rotation(rotation) {
        }

        Transform(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
            : position(position), rotation(rotation), scale(scale) {
        }

        explicit Transform(const Matrix4 &matrix) {
            decompose_matrix(matrix, position, rotation, scale);
        }

        Matrix4 to_matrix() const {
            Matrix4 rot_matrix{rotation.to_rotation_matrix()};
            Matrix4 scale_matrix = Matrix4::scale(scale);
            Matrix4 trans_matrix = Matrix4::translation(position);

            return trans_matrix * rot_matrix * scale_matrix;
        }

        Vector3 transform_point(const Vector3 &point) const {
            Vector3 scaled_point = point * scale;
            Vector3 rotated_point = rotation * scaled_point;

            return position + rotated_point;
        }

        Vector3 transform_direction(const Vector3 &direction) const {
            return rotation * direction;
        }

        Vector3 inverse_transform_point(const Vector3 &point) const {
            Vector3 local_point = point - position;

            Quaternion inv_rotation = rotation.inverse();
            Vector3 unrotated_point = inv_rotation * local_point;

            Vector3 inv_scale = Vector3(
                scale.x != 0.0f ? 1.0f / scale.x : 0.0f,
                scale.y != 0.0f ? 1.0f / scale.y : 0.0f,
                scale.z != 0.0f ? 1.0f / scale.z : 0.0f
            );

            return unrotated_point * inv_scale;
        }

        Vector3 inverse_transform_direction(const Vector3 &direction) const {
            return rotation.inverse() * direction;
        }

        Vector3 forward() const {
            return rotation * Vector3::forward();
        }

        Vector3 right() const {
            return rotation * Vector3::right();
        }

        Vector3 up() const {
            return rotation * Vector3::up();
        }

        void look_at(const Vector3 &target, const Vector3 &up = Vector3::up()) {
            Vector3 direction = target - position;
            if (direction.length_squared() > 0.0001f) {
                rotation = Quaternion::look_rotation(direction, up);
            }
        }

        static Transform lerp(const Transform &a, const Transform &b, float t) {
            return Transform(
                softcube::lerp(a.position, b.position, t),
                softcube::lerp(a.rotation, b.rotation, t),
                softcube::lerp(a.scale, b.scale, t)
            );
        }

        Transform combine(const Transform &local) const {
            Transform result;

            result.scale = scale * local.scale;
            result.rotation = rotation * local.rotation;
            result.position = position + (rotation * (local.position * scale));

            return result;
        }

        Transform inverse() const {
            Transform result;

            result.rotation = rotation.inverse();

            result.scale = Vector3(
                scale.x != 0.0f ? 1.0f / scale.x : 0.0f,
                scale.y != 0.0f ? 1.0f / scale.y : 0.0f,
                scale.z != 0.0f ? 1.0f / scale.z : 0.0f
            );

            Vector3 inv_pos = -position;
            result.position = result.rotation * (inv_pos * result.scale);

            return result;
        }

        static void decompose_matrix(const Matrix4 &matrix, Vector3 &position, Quaternion &rotation, Vector3 &scale) {
            position = matrix.get_translation();
            scale = matrix.get_scale();

            Vector3 x_axis = Vector3(matrix.m00, matrix.m10, matrix.m20).normalized();
            Vector3 y_axis = Vector3(matrix.m01, matrix.m11, matrix.m21).normalized();
            Vector3 z_axis = Vector3(matrix.m02, matrix.m12, matrix.m22).normalized();

            Matrix3 rot_matrix(
                x_axis.x, y_axis.x, z_axis.x,
                x_axis.y, y_axis.y, z_axis.y,
                x_axis.z, y_axis.z, z_axis.z
            );

            rotation = Quaternion::from_rotation_matrix(rot_matrix);
        }
    };
}
