#pragma once
#include "core/common.hpp"

namespace softcube {
    /**
     * @struct Vector2
     * @brief 2D vector with x and y components
     */
    struct Vector2 {
        float x{0.0f};
        float y{0.0f};

        Vector2() = default;

        Vector2(float x, float y) : x(x), y(y) {
        }

        explicit Vector2(float value) : x(value), y(value) {
        }

        Vector2(const Vector2 &) = default;

        Vector2 &operator=(const Vector2 &) = default;

        Vector2(Vector2 &&) = default;

        Vector2 &operator=(Vector2 &&) = default;

        static Vector2 zero() { return Vector2(0.0f, 0.0f); }
        static Vector2 one() { return Vector2(1.0f, 1.0f); }
        static Vector2 unit_x() { return Vector2(1.0f, 0.0f); }
        static Vector2 unit_y() { return Vector2(0.0f, 1.0f); }

        Vector2 operator+(const Vector2 &other) const {
            return Vector2(x + other.x, y + other.y);
        }

        Vector2 operator-(const Vector2 &other) const {
            return Vector2(x - other.x, y - other.y);
        }

        Vector2 operator*(float scalar) const {
            return Vector2(x * scalar, y * scalar);
        }

        Vector2 operator/(float scalar) const {
            return Vector2(x / scalar, y / scalar);
        }

        Vector2 operator*(const Vector2 &other) const {
            return Vector2(x * other.x, y * other.y);
        }

        Vector2 operator/(const Vector2 &other) const {
            return Vector2(x / other.x, y / other.y);
        }

        Vector2 operator-() const {
            return Vector2(-x, -y);
        }

        Vector2 &operator+=(const Vector2 &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2 &operator-=(const Vector2 &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2 &operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        Vector2 &operator*=(const Vector2 &other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vector2 &operator/=(const Vector2 &other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        bool operator==(const Vector2 &other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2 &other) const {
            return !(*this == other);
        }

        float length() const {
            return std::sqrt(x * x + y * y);
        }

        float length_squared() const {
            return x * x + y * y;
        }

        Vector2 normalized() const {
            float len = length();
            if (len > 0.0f) {
                return Vector2(x / len, y / len);
            }
            return *this;
        }

        void normalize() {
            float len = length();
            if (len > 0.0f) {
                x /= len;
                y /= len;
            }
        }

        float dot(const Vector2 &other) const {
            return x * other.x + y * other.y;
        }

        float cross(const Vector2 &other) const {
            return x * other.y - y * other.x;
        }

        float distance(const Vector2 &other) const {
            return (*this - other).length();
        }

        float distance_squared(const Vector2 &other) const {
            return (*this - other).length_squared();
        }

        Vector2 lerp(const Vector2 &other, float t) const {
            return Vector2(x + (other.x - x) * t, y + (other.y - y) * t);
        }

        Vector2 reflect(const Vector2 &normal) const {
            return *this - normal * (2.0f * dot(normal));
        }

        Vector2 perpendicular() const {
            return Vector2(-y, x);
        }
    };

    inline Vector2 operator*(float scalar, const Vector2 &vec) {
        return vec * scalar;
    }

    inline float dot(const Vector2 &a, const Vector2 &b) {
        return a.dot(b);
    }

    inline float cross(const Vector2 &a, const Vector2 &b) {
        return a.cross(b);
    }

    inline Vector2 normalize(const Vector2 &v) {
        return v.normalized();
    }

    inline Vector2 lerp(const Vector2 &a, const Vector2 &b, float t) {
        return a.lerp(b, t);
    }

    inline Vector2 reflect(const Vector2 &vec, const Vector2 &normal) {
        return vec.reflect(normal);
    }
}
