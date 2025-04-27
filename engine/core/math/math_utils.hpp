#pragma once
#include "core/common.hpp"
#include <cmath>
#include <random>

namespace softcube {
    namespace math {
        constexpr float PI = 3.14159265358979323846f;
        constexpr float TWO_PI = 2.0f * PI;
        constexpr float HALF_PI = PI / 2.0f;
        constexpr float QUARTER_PI = PI / 4.0f;
        constexpr float INV_PI = 1.0f / PI;
        constexpr float DEG_TO_RAD = PI / 180.0f;
        constexpr float RAD_TO_DEG = 180.0f / PI;
        constexpr float EPSILON = 1e-6f;

        template<typename T>
        T lerp(const T &a, const T &b, float t) {
            return a + (b - a) * t;
        }

        template<typename T>
        T clamp(const T &value, const T &min_value, const T &max_value) {
            return std::min(std::max(value, min_value), max_value);
        }

        inline float to_radians(float degrees) {
            return degrees * DEG_TO_RAD;
        }

        inline float to_degrees(float radians) {
            return radians * RAD_TO_DEG;
        }

        inline bool approximately(float a, float b, float epsilon = EPSILON) {
            return std::abs(a - b) < epsilon;
        }

        inline float smoothstep(float edge0, float edge1, float x) {
            x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return x * x * (3.0f - 2.0f * x);
        }

        inline float smootherstep(float edge0, float edge1, float x) {
            x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
        }

        inline float barycentric(float v1, float v2, float v3, float u, float v) {
            return v1 + (v2 - v1) * u + (v3 - v1) * v;
        }

        inline bool is_power_of_two(int x) {
            return (x != 0) && ((x & (x - 1)) == 0);
        }

        inline int next_power_of_two(int x) {
            x--;
            x |= x >> 1;
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            x |= x >> 16;
            return x + 1;
        }

        inline float random_float(const float min = 0.0f, const float max = 1.0f) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution dis(min, max);
            return dis(gen);
        }

        inline int random_int(const int min, const int max) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution dis(min, max);
            return dis(gen);
        }

        inline float hermite(const float y0, const float y1, const float y2, const float y3, const float mu) {
            float m0, m1, mu2, mu3;
            float a0, a1, a2, a3;

            mu2 = mu * mu;
            mu3 = mu2 * mu;
            m0 = (y1 - y0) * 0.5f;
            m0 += (y2 - y1) * 0.5f;
            m1 = (y2 - y1) * 0.5f;
            m1 += (y3 - y2) * 0.5f;
            a0 = 2.0f * mu3 - 3.0f * mu2 + 1.0f;
            a1 = mu3 - 2.0f * mu2 + mu;
            a2 = mu3 - mu2;
            a3 = -2.0f * mu3 + 3.0f * mu2;

            return a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2;
        }

        inline float catmull_rom(float y0, float y1, float y2, float y3, float mu) {
            float mu2 = mu * mu;
            float a0 = -0.5f * y0 + 1.5f * y1 - 1.5f * y2 + 0.5f * y3;
            float a1 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
            float a2 = -0.5f * y0 + 0.5f * y2;
            float a3 = y1;

            return a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3;
        }

        inline float bezier(float p0, float p1, float p2, float p3, float t) {
            float t1 = 1.0f - t;
            return t1 * t1 * t1 * p0 + 3.0f * t1 * t1 * t * p1 + 3.0f * t1 * t * t * p2 + t * t * t * p3;
        }

        inline float wrap(float value, float min, float max) {
            float range = max - min;
            return min + std::fmod(std::fmod(value - min, range) + range, range);
        }

        inline float ping_pong(float value, float length) {
            value = wrap(value, 0.0f, length * 2.0f);
            return length - std::abs(value - length);
        }

        template<typename T>
        int sign(T value) {
            return (T(0) < value) - (value < T(0));
        }
    }
}
