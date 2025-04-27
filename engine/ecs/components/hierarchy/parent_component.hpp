#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Parent
     * @brief Component that represents a parent-child relationship
     */
    struct Parent {
        entt::entity entity = entt::null;

        Parent() = default;

        explicit Parent(const entt::entity parent) : entity(parent) {
        }
    };
}