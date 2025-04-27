#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Velocity
     * @brief Component that holds velocity information for an entity
     */
    struct Velocity {
        Vector3 linear{0.0f, 0.0f, 0.0f};
        Vector3 angular{0.0f, 0.0f, 0.0f};

        Velocity() = default;

        explicit Velocity(const Vector3 &linear, const Vector3 &angular)
            : linear(linear), angular(angular) {
        }
    };
}