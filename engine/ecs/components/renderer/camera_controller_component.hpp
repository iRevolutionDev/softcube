#pragma once

#include "core/common.hpp"

namespace softcube::component {
    /**
     * @struct CameraController
     * @brief Component for camera movement and rotation controls
     */
    struct CameraController {
        float movement_speed = 5.0f;

        float rotation_speed = 0.002f;

        float zoom_speed = 2.0f;

        bool is_active = true;

        int forward_key = SDL_SCANCODE_W;
        int backward_key = SDL_SCANCODE_S;
        int left_key = SDL_SCANCODE_A;
        int right_key = SDL_SCANCODE_D;
        int up_key = SDL_SCANCODE_E;
        int down_key = SDL_SCANCODE_Q;

        float sensitivity = 1.0f;

        double last_mouse_x = 0.0;
        double last_mouse_y = 0.0;
        bool first_mouse = true;

        Vector3 orbit_target{0.0f, 0.0f, 0.0f};
        float orbit_distance = 10.0f;
        float orbit_min_distance = 0.5f;
        float orbit_max_distance = 100.0f;

        CameraController() = default;

        /**
         * @brief Set the orbit target and initial distance
         * @param target Point to orbit around
         * @param distance Initial distance from target
         */
        void set_orbit_target(const Vector3 &target, float distance = 10.0f) {
            orbit_target = target;
            orbit_distance = std::clamp(distance, orbit_min_distance, orbit_max_distance);
        }
    };
}
