#pragma once
#include "core/common.hpp"
#include "vector3.hpp"
#include "matrix.hpp"

namespace softcube {
    /**
     * @struct AABB
     * @brief Axis-Aligned Bounding Box for collision detection and spatial queries
     */
    struct AABB {
        Vector3 min;
        Vector3 max;

        AABB() : min(Vector3(std::numeric_limits<float>::max())),
                 max(Vector3(-std::numeric_limits<float>::max())) {
        }

        AABB(const Vector3 &min, const Vector3 &max) : min(min), max(max) {
        }

        static AABB from_center_and_extents(const Vector3 &center, const Vector3 &half_extents) {
            return {center - half_extents, center + half_extents};
        }

        static AABB from_points(const Vector3 *points, size_t count) {
            AABB result;
            for (size_t i = 0; i < count; ++i) {
                result.expand(points[i]);
            }
            return result;
        }

        [[nodiscard]] Vector3 center() const {
            return (min + max) * 0.5f;
        }

        [[nodiscard]] Vector3 extents() const {
            return (max - min) * 0.5f;
        }

        [[nodiscard]] Vector3 size() const {
            return max - min;
        }

        [[nodiscard]] float volume() const {
            Vector3 s = size();
            return s.x * s.y * s.z;
        }

        [[nodiscard]] float surface_area() const {
            Vector3 s = size();
            return 2.0f * (s.x * s.y + s.x * s.z + s.y * s.z);
        }

        [[nodiscard]] bool is_valid() const {
            return min.x <= max.x && min.y <= max.y && min.z <= max.z;
        }

        void get_corners(Vector3 corners[8]) const {
            corners[0] = min;
            corners[1] = Vector3(max.x, min.y, min.z);
            corners[2] = Vector3(max.x, max.y, min.z);
            corners[3] = Vector3(min.x, max.y, min.z);
            corners[4] = Vector3(min.x, min.y, max.z);
            corners[5] = Vector3(max.x, min.y, max.z);
            corners[6] = max;
            corners[7] = Vector3(min.x, max.y, max.z);
        }

        void expand(const Vector3 &point) {
            min.x = std::min(min.x, point.x);
            min.y = std::min(min.y, point.y);
            min.z = std::min(min.z, point.z);
            max.x = std::max(max.x, point.x);
            max.y = std::max(max.y, point.y);
            max.z = std::max(max.z, point.z);
        }

        void expand(const AABB &other) {
            min.x = std::min(min.x, other.min.x);
            min.y = std::min(min.y, other.min.y);
            min.z = std::min(min.z, other.min.z);
            max.x = std::max(max.x, other.max.x);
            max.y = std::max(max.y, other.max.y);
            max.z = std::max(max.z, other.max.z);
        }

        void expand(float amount) {
            min -= Vector3(amount);
            max += Vector3(amount);
        }

        [[nodiscard]] bool contains(const Vector3 &point) const {
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }

        [[nodiscard]] bool contains(const AABB &other) const {
            return min.x <= other.min.x && max.x >= other.max.x &&
                   min.y <= other.min.y && max.y >= other.max.y &&
                   min.z <= other.min.z && max.z >= other.max.z;
        }

        [[nodiscard]] bool intersects(const AABB &other) const {
            return max.x >= other.min.x && min.x <= other.max.x &&
                   max.y >= other.min.y && min.y <= other.max.y &&
                   max.z >= other.min.z && min.z <= other.max.z;
        }

        bool intersect_ray(const Vector3 &origin, const Vector3 &direction, float &t_min, float &t_max) const {
            float t1, t2, t_near = -std::numeric_limits<float>::max();
            float t_far = std::numeric_limits<float>::max();

            for (int i = 0; i < 3; i++) {
                float component_dir = (&direction.x)[i];
                float component_min = (&min.x)[i];
                float component_max = (&max.x)[i];
                float component_org = (&origin.x)[i];

                if (std::abs(component_dir) < 1e-8f) {
                    if (component_org < component_min || component_org > component_max) {
                        return false;
                    }
                } else {
                    float inv_dir = 1.0f / component_dir;
                    t1 = (component_min - component_org) * inv_dir;
                    t2 = (component_max - component_org) * inv_dir;

                    if (t1 > t2) {
                        std::swap(t1, t2);
                    }

                    t_near = std::max(t_near, t1);
                    t_far = std::min(t_far, t2);

                    if (t_near > t_far) {
                        return false;
                    }
                }
            }

            t_min = t_near;
            t_max = t_far;
            return true;
        }

        [[nodiscard]] AABB transform(const Matrix4 &matrix) const {
            Vector3 corners[8];
            get_corners(corners);

            AABB result;
            for (auto corner: corners) {
                result.expand(matrix.transform_point(corner));
            }

            return result;
        }

        static AABB merge(const AABB &a, const AABB &b) {
            return {
                Vector3(std::min(a.min.x, b.min.x),
                        std::min(a.min.y, b.min.y),
                        std::min(a.min.z, b.min.z)),
                Vector3(std::max(a.max.x, b.max.x),
                        std::max(a.max.y, b.max.y),
                        std::max(a.max.z, b.max.z))
            };
        }

        static AABB intersection(const AABB &a, const AABB &b) {
            const AABB result(
                Vector3(std::max(a.min.x, b.min.x),
                        std::max(a.min.y, b.min.y),
                        std::max(a.min.z, b.min.z)),
                Vector3(std::min(a.max.x, b.max.x),
                        std::min(a.max.y, b.max.y),
                        std::min(a.max.z, b.max.z))
            );

            if (result.min.x > result.max.x || result.min.y > result.max.y || result.min.z > result.max.z) {
                return {};
            }

            return result;
        }
    };
}
