#pragma once
#include "core/common.hpp"
#include "vector2.hpp"

namespace softcube {
    /**
     * @struct Vector4
     * @brief 4D vector with x, y, z, and w components
     */
    struct Vector4 {
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};
        float w{0.0f};

        Vector4() = default;

        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
        }

        explicit Vector4(float value) : x(value), y(value), z(value), w(value) {
        }

        Vector4(const Vector2 &v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {
        }

        Vector4(const Vector3 &v, float w) : x(v.x), y(v.y), z(v.z), w(w) {
        }

        Vector4(const Vector4 &) = default;

        Vector4 &operator=(const Vector4 &) = default;

        Vector4(Vector4 &&) = default;

        Vector4 &operator=(Vector4 &&) = default;

        static Vector4 zero() { return Vector4(0.0f, 0.0f, 0.0f, 0.0f); }
        static Vector4 one() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
        static Vector4 unit_x() { return Vector4(1.0f, 0.0f, 0.0f, 0.0f); }
        static Vector4 unit_y() { return Vector4(0.0f, 1.0f, 0.0f, 0.0f); }
        static Vector4 unit_z() { return Vector4(0.0f, 0.0f, 1.0f, 0.0f); }
        static Vector4 unit_w() { return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }

        Vector2 xy() const { return Vector2(x, y); }
        Vector3 xyz() const { return Vector3(x, y, z); }

        Vector4 operator+(const Vector4 &other) const {
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        Vector4 operator-(const Vector4 &other) const {
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        Vector4 operator*(float scalar) const {
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        Vector4 operator/(float scalar) const {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        Vector4 operator*(const Vector4 &other) const {
            return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
        }

        Vector4 operator/(const Vector4 &other) const {
            return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
        }

        Vector4 operator-() const {
            return Vector4(-x, -y, -z, -w);
        }

        Vector4 &operator+=(const Vector4 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Vector4 &operator-=(const Vector4 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Vector4 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Vector4 &operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        Vector4 &operator*=(const Vector4 &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }

        Vector4 &operator/=(const Vector4 &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        bool operator==(const Vector4 &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const Vector4 &other) const {
            return !(*this == other);
        }

        float length() const {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        float length_squared() const {
            return x * x + y * y + z * z + w * w;
        }

        Vector4 normalized() const {
            float len = length();
            if (len > 0.0f) {
                return Vector4(x / len, y / len, z / len, w / len);
            }
            return *this;
        }

        void normalize() {
            float len = length();
            if (len > 0.0f) {
                x /= len;
                y /= len;
                z /= len;
                w /= len;
            }
        }

        float dot(const Vector4 &other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        float distance(const Vector4 &other) const {
            return (*this - other).length();
        }

        float distance_squared(const Vector4 &other) const {
            return (*this - other).length_squared();
        }

        Vector4 lerp(const Vector4 &other, float t) const {
            return Vector4(
                x + (other.x - x) * t,
                y + (other.y - y) * t,
                z + (other.z - z) * t,
                w + (other.w - w) * t
            );
        }

        Vector3 homogenize() const {
            if (w != 0.0f) {
                float inv_w = 1.0f / w;
                return Vector3(x * inv_w, y * inv_w, z * inv_w);
            }
            return Vector3(x, y, z);
        }
    };

    inline Vector4 operator*(float scalar, const Vector4 &vec) {
        return vec * scalar;
    }

    inline float dot(const Vector4 &a, const Vector4 &b) {
        return a.dot(b);
    }

    inline Vector4 normalize(const Vector4 &v) {
        return v.normalized();
    }

    inline Vector4 lerp(const Vector4 &a, const Vector4 &b, float t) {
        return a.lerp(b, t);
    }

    inline Vector4 min(const Vector4 &a, const Vector4 &b) {
        return Vector4(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z),
            std::min(a.w, b.w)
        );
    }

    inline Vector4 max(const Vector4 &a, const Vector4 &b) {
        return Vector4(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z),
            std::max(a.w, b.w)
        );
    }

    inline Vector4 clamp(const Vector4 &value, const Vector4 &min_value, const Vector4 &max_value) {
        return Vector4(
            std::clamp(value.x, min_value.x, max_value.x),
            std::clamp(value.y, min_value.y, max_value.y),
            std::clamp(value.z, min_value.z, max_value.z),
            std::clamp(value.w, min_value.w, max_value.w)
        );
    }
}
