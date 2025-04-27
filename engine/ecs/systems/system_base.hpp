#pragma once
#include "core/common.hpp"

namespace softcube::system {
    /**
     * @class System
     * @brief Base class for all ECS systems
     * 
     * Systems are responsible for processing entities with specific components.
     * This base class provides common functionality for all systems.
     */
    class System {
    public:
        System() = default;
        virtual ~System() = default;
        
        /**
         * @brief Initialize the system
         * @param registry Reference to the EnTT registry
         */
        virtual void init(entt::registry& registry) {
            m_registry = &registry;
        }
        
        /**
         * @brief Update the system
         * @param dt Delta time in seconds
         */
        virtual void update(float dt) = 0;
        
        /**
         * @brief Get the registry
         * @return Pointer to the EnTT registry
         */
        entt::registry* get_registry() const { return m_registry; }
        
        /**
         * @brief Set whether the system is enabled
         * @param enabled Whether the system is enabled
         */
        void set_enabled(const bool enabled) { m_enabled = enabled; }
        
        /**
         * @brief Check if the system is enabled
         * @return True if the system is enabled, false otherwise
         */
        bool is_enabled() const { return m_enabled; }
        
    protected:
        entt::registry* m_registry = nullptr;
        bool m_enabled = true;
    };
}
