#pragma once
#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Tag
     * @brief A simple tag component to identify entities
     */
    struct Tag {
        std::string tag;

        Tag() = default;

        explicit Tag(const std::string &tag) : tag(tag) {
        }
    };
}
