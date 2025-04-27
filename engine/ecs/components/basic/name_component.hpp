#pragma once
#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct Name
     * @brief A simple name component to identify entities
     */
    struct Name {
        std::string name;

        Name() = default;

        explicit Name(const std::string &name) : name(name) {
        }
    };
}