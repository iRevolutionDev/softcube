#pragma once
#include "core/common.hpp"
#include "vector2.hpp"

namespace softcube {
    struct Quaternion;
}

namespace softcube {
    /**
     * @struct Vector3
     * @brief 3D vector with x, y, and z components
     */
    struct Vector3 {
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};

        Vector3() = default;

        Vector3(float x, float y, float z) : x(x), y(y), z(z) {
        }

        explicit Vector3(float value) : x(value), y(value), z(value) {
        }

        Vector3(const Vector2 &v, float z) : x(v.x), y(v.y), z(z) {
        }

        Vector3(const Vector3 &) = default;

        Vector3 &operator=(const Vector3 &) = default;

        Vector3 &&operator*(const Quaternion &rotation) const;

        Vector3(Vector3 &&) = default;

        Vector3 &operator=(Vector3 &&) = default;

        Vector3(const bx::Vec3 &vec) : x(vec.x), y(vec.y), z(vec.z) {
        }

        operator bx::Vec3() const { return {x, y, z}; }

        static Vector3 zero() { return Vector3(0.0f, 0.0f, 0.0f); }
        static Vector3 one() { return Vector3(1.0f, 1.0f, 1.0f); }
        static Vector3 unit_x() { return Vector3(1.0f, 0.0f, 0.0f); }
        static Vector3 unit_y() { return Vector3(0.0f, 1.0f, 0.0f); }
        static Vector3 unit_z() { return Vector3(0.0f, 0.0f, 1.0f); }
        static Vector3 forward() { return Vector3(0.0f, 0.0f, 1.0f); }
        static Vector3 back() { return Vector3(0.0f, 0.0f, -1.0f); }
        static Vector3 up() { return Vector3(0.0f, 1.0f, 0.0f); }
        static Vector3 down() { return Vector3(0.0f, -1.0f, 0.0f); }
        static Vector3 right() { return Vector3(1.0f, 0.0f, 0.0f); }
        static Vector3 left() { return Vector3(-1.0f, 0.0f, 0.0f); }

        Vector2 xy() const { return Vector2(x, y); }
        Vector2 xz() const { return Vector2(x, z); }
        Vector2 yz() const { return Vector2(y, z); }

        Vector3 operator+(const Vector3 &other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        Vector3 operator-(const Vector3 &other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator/(float scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        Vector3 operator*(const Vector3 &other) const {
            return Vector3(x * other.x, y * other.y, z * other.z);
        }

        Vector3 operator/(const Vector3 &other) const {
            return Vector3(x / other.x, y / other.y, z / other.z);
        }

        Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }

        Vector3 &operator+=(const Vector3 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3 &operator-=(const Vector3 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector3 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3 &operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        Vector3 &operator*=(const Vector3 &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        Vector3 &operator/=(const Vector3 &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        bool operator==(const Vector3 &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3 &other) const {
            return !(*this == other);
        }

        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        float length_squared() const {
            return x * x + y * y + z * z;
        }

        Vector3 normalized() const {
            float len = length();
            if (len > 0.0f) {
                return Vector3(x / len, y / len, z / len);
            }
            return *this;
        }

        void normalize() {
            float len = length();
            if (len > 0.0f) {
                x /= len;
                y /= len;
                z /= len;
            }
        }

        float dot(const Vector3 &other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        Vector3 cross(const Vector3 &other) const {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        float distance(const Vector3 &other) const {
            return (*this - other).length();
        }

        float distance_squared(const Vector3 &other) const {
            return (*this - other).length_squared();
        }

        Vector3 lerp(const Vector3 &other, float t) const {
            return Vector3(
                x + (other.x - x) * t,
                y + (other.y - y) * t,
                z + (other.z - z) * t
            );
        }

        Vector3 reflect(const Vector3 &normal) const {
            return *this - normal * (2.0f * dot(normal));
        }

        Vector3 project(const Vector3 &normal) const {
            return normal * (dot(normal) / normal.length_squared());
        }

        Vector3 project_onto_plane(const Vector3 &normal) const {
            return *this - project(normal);
        }

        float angle(const Vector3 &other) const {
            float cos_angle = dot(other) / (length() * other.length());
            cos_angle = std::clamp(cos_angle, -1.0f, 1.0f);
            return std::acos(cos_angle);
        }

        bool is_zero() const {
            return x == 0.0f && y == 0.0f && z == 0.0f;
        }

        Vector3 rotate_around_axis(const Vector3 &axis, float angle) const;
    };

    inline Vector3 operator*(float scalar, const Vector3 &vec) {
        return vec * scalar;
    }

    inline float dot(const Vector3 &a, const Vector3 &b) {
        return a.dot(b);
    }

    inline Vector3 cross(const Vector3 &a, const Vector3 &b) {
        return a.cross(b);
    }

    inline Vector3 normalize(const Vector3 &v) {
        return v.normalized();
    }

    inline Vector3 lerp(const Vector3 &a, const Vector3 &b, float t) {
        return a.lerp(b, t);
    }

    inline Vector3 reflect(const Vector3 &vec, const Vector3 &normal) {
        return vec.reflect(normal);
    }

    inline Vector3 project(const Vector3 &vec, const Vector3 &onto) {
        return vec.project(onto);
    }

    inline float angle(const Vector3 &a, const Vector3 &b) {
        return a.angle(b);
    }

    inline Vector3 min(const Vector3 &a, const Vector3 &b) {
        return Vector3(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
    }

    inline Vector3 max(const Vector3 &a, const Vector3 &b) {
        return Vector3(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
    }

    inline Vector3 clamp(const Vector3 &value, const Vector3 &min_value, const Vector3 &max_value) {
        return Vector3(
            std::clamp(value.x, min_value.x, max_value.x),
            std::clamp(value.y, min_value.y, max_value.y),
            std::clamp(value.z, min_value.z, max_value.z)
        );
    }
}
