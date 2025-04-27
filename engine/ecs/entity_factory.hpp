#pragma once
#include "core/common.hpp"

namespace softcube {
    /**
     * @class EntityFactory
     * @brief Factory class for creating common entity types
     * 
     * This class provides helper methods to create common entity types
     * such as cameras, lights, 3D objects, and UI elements.
     */
    class EntityFactory {
    public:
        explicit EntityFactory(entt::registry *registry);

    /**
       * @brief Create a camera entity
       * @param position Camera position
       * @param is_main Whether this is the main camera
       * @return The created camera entity
       */
        Entity create_camera(const Vector3 &position, bool is_main = false) const;

        /**
         * @brief Create a cube primitive entity
         * @param position Cube position
         * @param size Size of the cube (edge length)
         * @param color Color of the cube (RGBA)
         * @return The created cube entity
         */
        Entity create_cube(const Vector3 &position, float size = 1.0f, const Vector4 &color = Vector4(1.0f, 1.0f, 1.0f, 1.0f)) const;

    private:
        entt::registry *m_registry;
    };
}
