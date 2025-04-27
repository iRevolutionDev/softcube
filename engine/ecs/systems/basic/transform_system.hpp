#pragma once
#include "ecs/systems/system_base.hpp"

namespace softcube::system {
    /**
     * @class TransformSystem
     * @brief System for updating transform hierarchies
     * 
     * This system handles parent-child relationships between transforms
     * and ensures that child entities inherit their parent's transform.
     * Note: This system handles transforms with direct parent references.
     * For transforms with Parent components, use the HierarchySystem.
     */
    class TransformSystem final : public System {
    public:
        TransformSystem() = default;

        void update(float dt) override;
    };
}
