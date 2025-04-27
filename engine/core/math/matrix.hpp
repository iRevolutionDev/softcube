#pragma once
#include "core/common.hpp"
#include "vector3.hpp"
#include "vector4.hpp"

namespace softcube {
    /**
     * @struct Matrix3
     * @brief 3x3 matrix for 3D transformations
     */
    struct Matrix3 {
        union {
            struct {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
            };

            float m[3][3];
            float values[9];
        };

        Matrix3() {
            *this = identity();
        }

        Matrix3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22
        ) : m00(m00), m01(m01), m02(m02),
            m10(m10), m11(m11), m12(m12),
            m20(m20), m21(m21), m22(m22) {
        }

        static Matrix3 identity() {
            return Matrix3(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            );
        }

        static Matrix3 zero() {
            return Matrix3(
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f
            );
        }

        static Matrix3 scale(const float x, const float y, const float z) {
            return Matrix3(
                x, 0.0f, 0.0f,
                0.0f, y, 0.0f,
                0.0f, 0.0f, z
            );
        }

        static Matrix3 scale(const Vector3 &s) {
            return scale(s.x, s.y, s.z);
        }

        static Matrix3 rotation_x(const float angle_radians) {
            float c = std::cos(angle_radians);
            float s = std::sin(angle_radians);
            return Matrix3(
                1.0f, 0.0f, 0.0f,
                0.0f, c, -s,
                0.0f, s, c
            );
        }

        static Matrix3 rotation_y(const float angle_radians) {
            float c = std::cos(angle_radians);
            float s = std::sin(angle_radians);
            return Matrix3(
                c, 0.0f, s,
                0.0f, 1.0f, 0.0f,
                -s, 0.0f, c
            );
        }

        static Matrix3 rotation_z(float angle_radians) {
            float c = std::cos(angle_radians);
            float s = std::sin(angle_radians);
            return Matrix3(
                c, -s, 0.0f,
                s, c, 0.0f,
                0.0f, 0.0f, 1.0f
            );
        }

        static Matrix3 rotation_axis(const Vector3 &axis, float angle_radians) {
            const auto c = std::cos(angle_radians);
            const auto s = std::sin(angle_radians);
            const auto t = 1.0f - c;

            const auto normalized_axis = normalize(axis);
            const auto x = normalized_axis.x;
            const auto y = normalized_axis.y;
            const auto z = normalized_axis.z;

            return Matrix3(
                t * x * x + c, t * x * y - s * z, t * x * z + s * y,
                t * x * y + s * z, t * y * y + c, t * y * z - s * x,
                t * x * z - s * y, t * y * z + s * x, t * z * z + c
            );
        }

        Matrix3 transpose() const {
            return {
                m00, m01, m02,
                m10, m11, m12,
                m20, m21, m22
            };
        }

        float determinant() const {
            return m00 * (m11 * m22 - m12 * m21)
                   - m01 * (m10 * m22 - m12 * m20)
                   + m02 * (m10 * m21 - m11 * m20);
        }

        Matrix3 inverse() const {
            const float det = determinant();
            if (std::abs(det) < 1e-6f) {
                return identity();
            }

            const float inv_det = 1.0f / det;

            Matrix3 result;
            result.m00 = (m11 * m22 - m12 * m21) * inv_det;
            result.m01 = (m02 * m21 - m01 * m22) * inv_det;
            result.m02 = (m01 * m12 - m02 * m11) * inv_det;
            result.m10 = (m12 * m20 - m10 * m22) * inv_det;
            result.m11 = (m00 * m22 - m02 * m20) * inv_det;
            result.m12 = (m02 * m10 - m00 * m12) * inv_det;
            result.m20 = (m10 * m21 - m11 * m20) * inv_det;
            result.m21 = (m01 * m20 - m00 * m21) * inv_det;
            result.m22 = (m00 * m11 - m01 * m10) * inv_det;

            return result;
        }

        Vector3 transform_vector(const Vector3 &v) const {
            return Vector3(
                m00 * v.x + m01 * v.y + m02 * v.z,
                m10 * v.x + m11 * v.y + m12 * v.z,
                m20 * v.x + m21 * v.y + m22 * v.z
            );
        }

        Matrix3 operator+(const Matrix3 &other) const {
            Matrix3 result;
            for (int i = 0; i < 9; ++i) {
                result.values[i] = values[i] + other.values[i];
            }
            return result;
        }

        Matrix3 operator-(const Matrix3 &other) const {
            Matrix3 result;
            for (int i = 0; i < 9; ++i) {
                result.values[i] = values[i] - other.values[i];
            }
            return result;
        }

        Matrix3 operator*(const Matrix3 &other) const {
            Matrix3 result = zero();
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    for (int k = 0; k < 3; ++k) {
                        result.m[i][j] += m[i][k] * other.m[k][j];
                    }
                }
            }
            return result;
        }

        Matrix3 operator*(float scalar) const {
            Matrix3 result;
            for (int i = 0; i < 9; ++i) {
                result.values[i] = values[i] * scalar;
            }
            return result;
        }

        Vector3 operator*(const Vector3 &v) const {
            return transform_vector(v);
        }

        Matrix3 &operator+=(const Matrix3 &other) {
            for (int i = 0; i < 9; ++i) {
                values[i] += other.values[i];
            }
            return *this;
        }

        Matrix3 &operator-=(const Matrix3 &other) {
            for (int i = 0; i < 9; ++i) {
                values[i] -= other.values[i];
            }
            return *this;
        }

        Matrix3 &operator*=(const Matrix3 &other) {
            *this = *this * other;
            return *this;
        }

        Matrix3 &operator*=(float scalar) {
            for (int i = 0; i < 9; ++i) {
                values[i] *= scalar;
            }
            return *this;
        }

        bool operator==(const Matrix3 &other) const {
            for (int i = 0; i < 9; ++i) {
                if (values[i] != other.values[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const Matrix3 &other) const {
            return !(*this == other);
        }
    };

    inline Matrix3 operator*(const float scalar, const Matrix3 &matrix) {
        return matrix * scalar;
    }

    /**
     * @struct Matrix4
     * @brief 4x4 matrix for 3D transformations with homogeneous coordinates
     */
    struct Matrix4 {
        union {
            struct {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
            };

            float m[4][4];
            float values[16];
        };

        Matrix4() {
            *this = identity();
        }

        Matrix4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        ) : m00(m00), m01(m01), m02(m02), m03(m03),
            m10(m10), m11(m11), m12(m12), m13(m13),
            m20(m20), m21(m21), m22(m22), m23(m23),
            m30(m30), m31(m31), m32(m32), m33(m33) {
        }

        explicit Matrix4(const Matrix3 &m3) {
            m00 = m3.m00;
            m01 = m3.m01;
            m02 = m3.m02;
            m03 = 0.0f;
            m10 = m3.m10;
            m11 = m3.m11;
            m12 = m3.m12;
            m13 = 0.0f;
            m20 = m3.m20;
            m21 = m3.m21;
            m22 = m3.m22;
            m23 = 0.0f;
            m30 = 0.0f;
            m31 = 0.0f;
            m32 = 0.0f;
            m33 = 1.0f;
        }

        static Matrix4 identity() {
            return Matrix4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        static Matrix4 zero() {
            return Matrix4(
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f
            );
        }

        static Matrix4 translation(float x, float y, float z) {
            return Matrix4(
                1.0f, 0.0f, 0.0f, x,
                0.0f, 1.0f, 0.0f, y,
                0.0f, 0.0f, 1.0f, z,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        static Matrix4 translation(const Vector3 &t) {
            return translation(t.x, t.y, t.z);
        }

        static Matrix4 scale(const float x, const float y, const float z) {
            return Matrix4(
                x, 0.0f, 0.0f, 0.0f,
                0.0f, y, 0.0f, 0.0f,
                0.0f, 0.0f, z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        static Matrix4 scale(const Vector3 &s) {
            return scale(s.x, s.y, s.z);
        }

        static Matrix4 rotation_x(const float angle_radians) {
            const float c = std::cos(angle_radians);
            const float s = std::sin(angle_radians);
            return Matrix4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, c, -s, 0.0f,
                0.0f, s, c, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        static Matrix4 rotation_y(float angle_radians) {
            float c = std::cos(angle_radians);
            float s = std::sin(angle_radians);
            return Matrix4(
                c, 0.0f, s, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -s, 0.0f, c, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        static Matrix4 rotation_z(float angle_radians) {
            float c = std::cos(angle_radians);
            float s = std::sin(angle_radians);
            return Matrix4(
                c, -s, 0.0f, 0.0f,
                s, c, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        static Matrix4 rotation_axis(const Vector3 &axis, float angle_radians) {
            Matrix3 rot3 = Matrix3::rotation_axis(axis, angle_radians);
            return Matrix4(rot3);
        }

        static Matrix4 perspective(float fov_y, float aspect_ratio, float near_plane, float far_plane) {
            float tan_half_fov = std::tan(fov_y * 0.5f);

            Matrix4 result = zero();
            result.m00 = 1.0f / (aspect_ratio * tan_half_fov);
            result.m11 = 1.0f / tan_half_fov;
            result.m22 = -(far_plane + near_plane) / (far_plane - near_plane);
            result.m23 = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
            result.m32 = -1.0f;

            return result;
        }

        static Matrix4 orthographic(float left, float right, float bottom, float top, float near_plane,
                                    float far_plane) {
            float inv_width = 1.0f / (right - left);
            float inv_height = 1.0f / (top - bottom);
            float inv_depth = 1.0f / (far_plane - near_plane);

            Matrix4 result = identity();
            result.m00 = 2.0f * inv_width;
            result.m11 = 2.0f * inv_height;
            result.m22 = -2.0f * inv_depth;
            result.m03 = -(right + left) * inv_width;
            result.m13 = -(top + bottom) * inv_height;
            result.m23 = -(far_plane + near_plane) * inv_depth;

            return result;
        }

        static Matrix4 look_at(const Vector3 &eye, const Vector3 &target, const Vector3 &up) {
            Vector3 f = normalize(target - eye);
            Vector3 r = normalize(cross(f, normalize(up)));
            Vector3 u = cross(r, f);

            Matrix4 result = identity();
            result.m00 = r.x;
            result.m10 = r.y;
            result.m20 = r.z;
            result.m01 = u.x;
            result.m11 = u.y;
            result.m21 = u.z;
            result.m02 = -f.x;
            result.m12 = -f.y;
            result.m22 = -f.z;
            result.m03 = -dot(r, eye);
            result.m13 = -dot(u, eye);
            result.m23 = dot(f, eye);

            return result;
        }

        Matrix4 transpose() const {
            return {
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33
            };
        }

        Matrix3 to_matrix3() const {
            return Matrix3(
                m00, m01, m02,
                m10, m11, m12,
                m20, m21, m22
            );
        }

        float determinant() const {
            return m00 * cofactor(0, 0)
                   + m01 * cofactor(0, 1)
                   + m02 * cofactor(0, 2)
                   + m03 * cofactor(0, 3);
        }

        float cofactor(const int row, const int col) const {
            const auto sign = ((row + col) % 2) ? -1.0f : 1.0f;
            return sign * minor(row, col);
        }

        float minor(const int row, const int col) const {
            int rows[3];
            int cols[3];
            int index = 0;

            for (int i = 0; i < 4; ++i) {
                if (i == row) continue;
                rows[index] = i;
                index++;
            }

            index = 0;
            for (int i = 0; i < 4; ++i) {
                if (i == col) continue;
                cols[index] = i;
                index++;
            }

            float a = m[rows[0]][cols[0]];
            float b = m[rows[0]][cols[1]];
            float c = m[rows[0]][cols[2]];
            float d = m[rows[1]][cols[0]];
            float e = m[rows[1]][cols[1]];
            float f = m[rows[1]][cols[2]];
            float g = m[rows[2]][cols[0]];
            float h = m[rows[2]][cols[1]];
            float i = m[rows[2]][cols[2]];

            return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
        }

        Matrix4 inverse() const {
            float det = determinant();
            if (std::abs(det) < 1e-6f) {
                return identity();
            }

            float inv_det = 1.0f / det;
            Matrix4 result;

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.m[j][i] = cofactor(i, j) * inv_det;
                }
            }

            return result;
        }

        Vector3 get_translation() const {
            return Vector3(m03, m13, m23);
        }

        Vector3 get_scale() const {
            return Vector3(
                Vector3(m00, m10, m20).length(),
                Vector3(m01, m11, m21).length(),
                Vector3(m02, m12, m22).length()
            );
        }

        Vector3 transform_point(const Vector3 &v) const {
            Vector4 transformed = transform_vector(Vector4(v, 1.0f));
            if (std::abs(transformed.w) > 1e-6f) {
                float inv_w = 1.0f / transformed.w;
                return Vector3(transformed.x * inv_w, transformed.y * inv_w, transformed.z * inv_w);
            }
            return Vector3(transformed.x, transformed.y, transformed.z);
        }

        Vector3 transform_vector(const Vector3 &v) const {
            return Vector3(
                m00 * v.x + m01 * v.y + m02 * v.z,
                m10 * v.x + m11 * v.y + m12 * v.z,
                m20 * v.x + m21 * v.y + m22 * v.z
            );
        }

        Vector3 transform_direction(const Vector3 &v) const {
            return normalize(transform_vector(v));
        }

        Vector4 transform_vector(const Vector4 &v) const {
            return Vector4(
                m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w,
                m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w,
                m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w,
                m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w
            );
        }

        Matrix4 operator+(const Matrix4 &other) const {
            Matrix4 result;
            for (int i = 0; i < 16; ++i) {
                result.values[i] = values[i] + other.values[i];
            }
            return result;
        }

        Matrix4 operator-(const Matrix4 &other) const {
            Matrix4 result;
            for (int i = 0; i < 16; ++i) {
                result.values[i] = values[i] - other.values[i];
            }
            return result;
        }

        Matrix4 operator*(const Matrix4 &other) const {
            Matrix4 result = zero();
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        result.m[i][j] += m[i][k] * other.m[k][j];
                    }
                }
            }
            return result;
        }

        Matrix4 operator*(float scalar) const {
            Matrix4 result;
            for (int i = 0; i < 16; ++i) {
                result.values[i] = values[i] * scalar;
            }
            return result;
        }

        Vector4 operator*(const Vector4 &v) const {
            return transform_vector(v);
        }

        Matrix4 &operator+=(const Matrix4 &other) {
            for (int i = 0; i < 16; ++i) {
                values[i] += other.values[i];
            }
            return *this;
        }

        Matrix4 &operator-=(const Matrix4 &other) {
            for (int i = 0; i < 16; ++i) {
                values[i] -= other.values[i];
            }
            return *this;
        }

        Matrix4 &operator*=(const Matrix4 &other) {
            *this = *this * other;
            return *this;
        }

        Matrix4 &operator*=(float scalar) {
            for (int i = 0; i < 16; ++i) {
                values[i] *= scalar;
            }
            return *this;
        }

        bool operator==(const Matrix4 &other) const {
            for (int i = 0; i < 16; ++i) {
                if (values[i] != other.values[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const Matrix4 &other) const {
            return !(*this == other);
        }
    };

    inline Matrix4 operator*(const float scalar, const Matrix4 &matrix) {
        return matrix * scalar;
    }
}
