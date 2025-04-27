#pragma once
#include "core/common.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix.hpp"
#include "math_utils.hpp"

namespace softcube {
    /**
     * @struct Quaternion
     * @brief Quaternion for representing rotations in 3D space
     */
    struct Quaternion {
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};
        float w{1.0f};

        Quaternion() = default;

        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
        }

        Quaternion(const Quaternion &) = default;

        Quaternion &operator=(const Quaternion &) = default;

        Quaternion(Quaternion &&) = default;

        Quaternion &operator=(Quaternion &&) = default;

        explicit Quaternion(const bx::Quaternion &q) : x(q.x), y(q.y), z(q.z), w(q.w) {
        }

        explicit operator bx::Quaternion() const { return {x, y, z, w}; }

        static Quaternion identity() {
            return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Quaternion from_axis_angle(const Vector3 &axis, const float angle) {
            const auto normalized_axis = axis.normalized();
            const float half_angle = angle * 0.5f;
            const float sin_half_angle = std::sin(half_angle);

            return Quaternion(
                normalized_axis.x * sin_half_angle,
                normalized_axis.y * sin_half_angle,
                normalized_axis.z * sin_half_angle,
                std::cos(half_angle)
            );
        }

        static Quaternion from_euler(float pitch, float yaw, float roll) {
            const auto half_pitch = pitch * 0.5f;
            const auto half_yaw = yaw * 0.5f;
            const auto half_roll = roll * 0.5f;

            const auto cy = std::cos(half_yaw);
            const auto sy = std::sin(half_yaw);
            const auto cp = std::cos(half_pitch);
            const auto sp = std::sin(half_pitch);
            const auto cr = std::cos(half_roll);
            const auto sr = std::sin(half_roll);

            return Quaternion(
                cy * sp * cr + sy * cp * sr,
                cy * cp * sr - sy * sp * cr,
                sy * cp * cr - cy * sp * sr,
                cy * cp * cr + sy * sp * sr
            );
        }

        static Quaternion from_euler(const Vector3 &euler_angles) {
            return from_euler(euler_angles.x, euler_angles.y, euler_angles.z);
        }

        static Quaternion from_rotation_matrix(const Matrix3 &matrix) {
            if (const auto trace = matrix.m00 + matrix.m11 + matrix.m22; trace > 0.0f) {
                const auto s = 0.5f / std::sqrt(trace + 1.0f);
                return Quaternion(
                    (matrix.m21 - matrix.m12) * s,
                    (matrix.m02 - matrix.m20) * s,
                    (matrix.m10 - matrix.m01) * s,
                    0.25f / s
                );
            }

            if (matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22) {
                const auto s = 2.0f * std::sqrt(1.0f + matrix.m00 - matrix.m11 - matrix.m22);
                return Quaternion(
                    0.25f * s,
                    (matrix.m01 + matrix.m10) / s,
                    (matrix.m02 + matrix.m20) / s,
                    (matrix.m21 - matrix.m12) / s
                );
            }

            if (matrix.m11 > matrix.m22) {
                const auto s = 2.0f * std::sqrt(1.0f + matrix.m11 - matrix.m00 - matrix.m22);
                return Quaternion(
                    (matrix.m01 + matrix.m10) / s,
                    0.25f * s,
                    (matrix.m12 + matrix.m21) / s,
                    (matrix.m02 - matrix.m20) / s
                );
            }

            const auto s = 2.0f * std::sqrt(1.0f + matrix.m22 - matrix.m00 - matrix.m11);
            return Quaternion(
                (matrix.m02 + matrix.m20) / s,
                (matrix.m12 + matrix.m21) / s,
                0.25f * s,
                (matrix.m10 - matrix.m01) / s
            );
        }

        static Quaternion look_rotation(const Vector3 &forward, const Vector3 &up = Vector3::up()) {
            const auto forward_normalized = forward.normalized();
            const auto right = cross(up, forward_normalized).normalized();
            const auto up_orthogonal = cross(forward_normalized, right);

            Matrix3 rot_matrix;
            rot_matrix.m00 = right.x;
            rot_matrix.m01 = right.y;
            rot_matrix.m02 = right.z;
            rot_matrix.m10 = up_orthogonal.x;
            rot_matrix.m11 = up_orthogonal.y;
            rot_matrix.m12 = up_orthogonal.z;
            rot_matrix.m20 = forward_normalized.x;
            rot_matrix.m21 = forward_normalized.y;
            rot_matrix.m22 = forward_normalized.z;

            return from_rotation_matrix(rot_matrix);
        }

        Matrix3 to_rotation_matrix() const {
            const float xx = x * x;
            const float xy = x * y;
            float xz = x * z;
            float xw = x * w;
            float yy = y * y;
            float yz = y * z;
            float yw = y * w;
            float zz = z * z;
            float zw = z * w;

            return Matrix3(
                1.0f - 2.0f * (yy + zz), 2.0f * (xy - zw), 2.0f * (xz + yw),
                2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - xw),
                2.0f * (xz - yw), 2.0f * (yz + xw), 1.0f - 2.0f * (xx + yy)
            );
        }

        Vector3 to_euler() const {
            float check = 2.0f * (w * y - z * x);

            if (std::abs(check) >= 0.99999f) {
                return Vector3(
                    0.0f,
                    check > 0.0f ? -math::PI / 2.0f : math::PI / 2.0f,
                    -2.0f * std::atan2(x, w)
                );
            }

            return Vector3(
                std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y)),
                std::asin(check),
                std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z))
            );
        }

        void to_axis_angle(Vector3 &axis, float &angle) const {
            if (std::abs(w) > 0.99999f) {
                axis = Vector3(1.0f, 0.0f, 0.0f);
                angle = 0.0f;
                return;
            }

            float squared_length = x * x + y * y + z * z;
            if (squared_length < 0.0001f) {
                axis = Vector3(1.0f, 0.0f, 0.0f);
                angle = 0.0f;
                return;
            }

            const float inv_length = 1.0f / std::sqrt(squared_length);
            axis = Vector3(x * inv_length, y * inv_length, z * inv_length);
            angle = 2.0f * std::acos(std::clamp(w, -1.0f, 1.0f));
        }

        float length() const {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        float length_squared() const {
            return x * x + y * y + z * z + w * w;
        }

        Quaternion normalized() const {
            float len = length();
            if (len > 0.0f) {
                float inv_len = 1.0f / len;
                return Quaternion(
                    x * inv_len,
                    y * inv_len,
                    z * inv_len,
                    w * inv_len
                );
            }
            return *this;
        }

        void normalize() {
            float len = length();
            if (len > 0.0f) {
                float inv_len = 1.0f / len;
                x *= inv_len;
                y *= inv_len;
                z *= inv_len;
                w *= inv_len;
            }
        }

        Quaternion conjugate() const {
            return Quaternion(-x, -y, -z, w);
        }

        Quaternion inverse() const {
            float len_sq = length_squared();
            if (len_sq > 0.0f) {
                float inv_len_sq = 1.0f / len_sq;
                return Quaternion(
                    -x * inv_len_sq,
                    -y * inv_len_sq,
                    -z * inv_len_sq,
                    w * inv_len_sq
                );
            }
            return identity();
        }

        float dot(const Quaternion &other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        Quaternion lerp(const Quaternion &other, float t) const {
            Quaternion result(
                x + (other.x - x) * t,
                y + (other.y - y) * t,
                z + (other.z - z) * t,
                w + (other.w - w) * t
            );
            return result.normalized();
        }

        Quaternion slerp(const Quaternion &other, float t) const {
            float dot_product = dot(other);

            Quaternion end = other;
            if (dot_product < 0.0f) {
                dot_product = -dot_product;
                end = Quaternion(-other.x, -other.y, -other.z, -other.w);
            }

            if (dot_product > 0.9995f) {
                return lerp(end, t);
            }

            float theta_0 = std::acos(std::clamp(dot_product, -1.0f, 1.0f));
            float theta = theta_0 * t;

            float sin_theta = std::sin(theta);
            float sin_theta_0 = std::sin(theta_0);

            float s0 = std::cos(theta) - dot_product * sin_theta / sin_theta_0;
            float s1 = sin_theta / sin_theta_0;

            return Quaternion(
                s0 * x + s1 * end.x,
                s0 * y + s1 * end.y,
                s0 * z + s1 * end.z,
                s0 * w + s1 * end.w
            );
        }

        Vector3 rotate_vector(const Vector3 &v) const {
            Vector3 q_vector(x, y, z);
            Vector3 cross1 = cross(q_vector, v);
            Vector3 cross2 = cross(q_vector, cross1);

            return v + (cross1 * (2.0f * w) + cross2 * 2.0f);
        }

        Quaternion operator*(const Quaternion &other) const {
            return Quaternion(
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w,
                w * other.w - x * other.x - y * other.y - z * other.z
            );
        }

        Vector3 operator*(const Vector3 &v) const {
            return rotate_vector(v);
        }

        Quaternion operator+(const Quaternion &other) const {
            return Quaternion(
                x + other.x,
                y + other.y,
                z + other.z,
                w + other.w
            );
        }

        Quaternion operator-(const Quaternion &other) const {
            return Quaternion(
                x - other.x,
                y - other.y,
                z - other.z,
                w - other.w
            );
        }

        Quaternion operator*(float scalar) const {
            return Quaternion(
                x * scalar,
                y * scalar,
                z * scalar,
                w * scalar
            );
        }

        Quaternion operator-() const {
            return Quaternion(-x, -y, -z, -w);
        }

        Quaternion &operator*=(const Quaternion &other) {
            *this = *this * other;
            return *this;
        }

        Quaternion &operator+=(const Quaternion &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Quaternion &operator-=(const Quaternion &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Quaternion &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        bool operator==(const Quaternion &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const Quaternion &other) const {
            return !(*this == other);
        }

        static Quaternion rotation_x(float angle) {
            float half_angle = angle * 0.5f;
            return Quaternion(std::sin(half_angle), 0.0f, 0.0f, std::cos(half_angle));
        }

        static Quaternion rotation_y(float angle) {
            float half_angle = angle * 0.5f;
            return Quaternion(0.0f, std::sin(half_angle), 0.0f, std::cos(half_angle));
        }

        static Quaternion rotation_z(float angle) {
            float half_angle = angle * 0.5f;
            return Quaternion(0.0f, 0.0f, std::sin(half_angle), std::cos(half_angle));
        }
    };

    inline Quaternion operator*(float scalar, const Quaternion &q) {
        return q * scalar;
    }

    inline float dot(const Quaternion &a, const Quaternion &b) {
        return a.dot(b);
    }

    inline Quaternion normalize(const Quaternion &q) {
        return q.normalized();
    }

    inline Quaternion lerp(const Quaternion &a, const Quaternion &b, float t) {
        return a.lerp(b, t);
    }

    inline Quaternion slerp(const Quaternion &a, const Quaternion &b, float t) {
        return a.slerp(b, t);
    }

    inline Vector3 Vector3::rotate_around_axis(const Vector3 &axis, const float angle) const {
        const auto rotation = Quaternion::from_axis_angle(axis, angle);
        return rotation * *this;
    }
}
