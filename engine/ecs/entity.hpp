#pragma once

#include "core/common.hpp"

namespace softcube {
    /**
     * @class Entity
     * @brief A wrapper around EnTT's entity handle
     *
     * This class provides a convenient interface for working with entities
     * in the ECS. It wraps an EnTT entity handle and provides methods for
     * adding, removing, and getting components.
     */
    class Entity {
    public:
        /**
         * @brief Construct a new Entity
         * @param handle The EnTT entity handle
         * @param registry Pointer to the registry that owns the entity
         */
        Entity(const entt::entity handle, entt::registry *registry)
            : m_entity_handle(handle), m_registry(registry) {
        }

        /**
         * @brief Default constructor for an invalid entity
         */
        Entity() = default;

        /**
         * @brief Add a component to the entity
         * @tparam T Component type
         * @tparam Args Constructor argument types
         * @param args Arguments to forward to component constructor
         * @return Reference to the added component
         */
        template<typename T, typename... Args>
        T &add_component(Args &&... args) {
            return m_registry->emplace<T>(m_entity_handle, std::forward<Args>(args)...);
        }

        /**
         * @brief Check if the entity has a component
         * @tparam T Component type
         * @return True if the entity has the component
         */
        template<typename T>
        bool has_component() const {
            return m_registry->all_of<T>(m_entity_handle);
        }

        /**
         * @brief Get a component from the entity
         * @tparam T Component type
         * @return Reference to the component
         */
        template<typename T>
        T &get_component() {
            return m_registry->get<T>(m_entity_handle);
        }

        /**
         * @brief Get a component from the entity (const)
         * @tparam T Component type
         * @return Const reference to the component
         */
        template<typename T>
        const T &get_component() const {
            return m_registry->get<T>(m_entity_handle);
        }

        /**
         * @brief Remove a component from the entity
         * @tparam T Component type
         */
        template<typename T>
        void remove_component() const {
            m_registry->remove<T>(m_entity_handle);
        }

        /**
         * @brief Get the underlying EnTT entity handle
         * @return The EnTT entity handle
         */
        entt::entity get_handle() const { return m_entity_handle; }

        /**
         * @brief Check if the entity is valid
         */
        explicit operator bool() const {
            return m_entity_handle != entt::null && m_registry && m_registry->valid(m_entity_handle);
        }

        /**
         * @brief Compare entities for equality
         * @param other The entity to compare with
         * @return True if the entities are the same
         */
        bool operator==(const Entity &other) const {
            return m_entity_handle == other.m_entity_handle && m_registry == other.m_registry;
        }

        /**
         * @brief Compare entities for inequality
         * @param other The entity to compare with
         * @return True if the entities are different
         */
        bool operator!=(const Entity &other) const {
            return !(*this == other);
        }

    private:
        entt::entity m_entity_handle = entt::null;
        entt::registry *m_registry = nullptr;
    };
}
