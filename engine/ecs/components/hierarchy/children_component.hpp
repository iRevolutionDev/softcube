#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Children
     * @brief Component that holds references to child entities
     */
    struct Children {
        std::vector<entt::entity> entities;

        Children() = default;

        explicit Children(const std::vector<entt::entity> &children) : entities(children) {
        }

        void add_child(const entt::entity child) {
            entities.push_back(child);
        }

        void remove_child(const entt::entity child) {
            if (const auto it = std::ranges::find(entities, child); it != entities.end()) {
                entities.erase(it);
            }
        }
    };
}