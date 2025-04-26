#include "input_manager.hpp"

#include <backends/imgui_impl_sdl3.h>

#include "core/window.hpp"
#include <SDL3/SDL.h>

namespace softcube {
    InputManager::InputManager() : window(nullptr), next_callback_id(0) {
        current_key_states.fill(KeyState::Released);
        previous_key_states.fill(KeyState::Released);

        mouse_button_states.fill(KeyState::Released);
        prev_mouse_button_states.fill(KeyState::Released);

        mouse_x = 0.0;
        mouse_y = 0.0;
        last_mouse_x = 0.0;
        last_mouse_y = 0.0;
        scroll_x = 0.0;
        scroll_y = 0.0;
    }

    InputManager::~InputManager() {
        key_callbacks.clear();
    }

    bool InputManager::init(Window *window) {
        SC_INFO("Initializing input manager");
        this->window = window;
        if (!this->window) {
            SC_ERROR("Failed to initialize input manager: window pointer is null");
            return false;
        }

        SC_INFO("Input manager initialized successfully");
        return true;
    }

    void InputManager::update() {
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;

        scroll_x = 0.0;
        scroll_y = 0.0;

        std::swap(previous_key_states, current_key_states);
        std::swap(prev_mouse_button_states, mouse_button_states);

        for (size_t i = 0; i < current_key_states.size(); ++i) {
            if (current_key_states[i] == KeyState::Pressed) {
                current_key_states[i] = KeyState::Held;
            }
        }

        for (size_t i = 0; i < mouse_button_states.size(); ++i) {
            if (mouse_button_states[i] == KeyState::Pressed) {
                mouse_button_states[i] = KeyState::Held;
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    SC_INFO("Received quit event, setting window to close");
                    window->set_should_close(true);
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    SC_DEBUG("Window resize event: {}x{}", event.window.data1, event.window.data2);
                    break;

                case SDL_EVENT_KEY_DOWN: {
                    if (event.key.repeat != 0 && event.key.repeat != 1) {
                        break;
                    }
                    if (const int key_code = event.key.key; key_code < 512) {
                        current_key_states[key_code] = KeyState::Pressed;

                        for (const auto &[id, callback]: key_callbacks) {
                            if (callback.key_code == key_code) {
                                callback.callback(key_code, KeyState::Pressed);
                            }
                        }
                    }

                    break;
                }
                case SDL_EVENT_KEY_UP: {
                    if (const auto key_code = event.key.key; key_code < 512) {
                        current_key_states[key_code] = KeyState::Released;

                        for (const auto &[id, callback]: key_callbacks) {
                            if (callback.key_code == key_code) {
                                callback.callback(key_code, KeyState::Released);
                            }
                        }
                    }
                }
                break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    if (const auto button = event.button.button - 1;
                        button >= 0 && button < static_cast<int>(MouseButton::Count)) {
                        mouse_button_states[button] = KeyState::Pressed;
                    }
                }
                break;

                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    if (const auto button = event.button.button - 1;
                        button >= 0 && button < static_cast<int>(MouseButton::Count)) {
                        mouse_button_states[button] = KeyState::Released;
                    }
                }
                break;

                case SDL_EVENT_MOUSE_MOTION:
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    scroll_x += event.wheel.x;
                    scroll_y += event.wheel.y;
                    break;

                default:
                    break;
            }
        }
    }

    bool InputManager::is_key_pressed(int keyCode) const {
        if (keyCode < 0 || keyCode >= 512) {
            return false;
        }

        return current_key_states[keyCode] == KeyState::Pressed ||
               current_key_states[keyCode] == KeyState::Held;
    }

    bool InputManager::is_key_just_pressed(int keyCode) const {
        if (keyCode < 0 || keyCode >= 512) {
            return false;
        }

        return current_key_states[keyCode] == KeyState::Pressed &&
               previous_key_states[keyCode] == KeyState::Released;
    }

    bool InputManager::is_key_just_released(int keyCode) const {
        if (keyCode < 0 || keyCode >= 512) {
            return false;
        }

        return current_key_states[keyCode] == KeyState::Released &&
               (previous_key_states[keyCode] == KeyState::Pressed ||
                previous_key_states[keyCode] == KeyState::Held);
    }

    bool InputManager::is_mouse_button_pressed(MouseButton button) const {
        int button_index = static_cast<int>(button);
        if (button_index < 0 || button_index >= static_cast<int>(MouseButton::Count)) {
            return false;
        }

        return mouse_button_states[button_index] == KeyState::Pressed ||
               mouse_button_states[button_index] == KeyState::Held;
    }

    bool InputManager::is_mouse_button_just_pressed(MouseButton button) const {
        int button_index = static_cast<int>(button);
        if (button_index < 0 || button_index >= static_cast<int>(MouseButton::Count)) {
            return false;
        }

        return mouse_button_states[button_index] == KeyState::Pressed &&
               prev_mouse_button_states[button_index] == KeyState::Released;
    }

    bool InputManager::is_mouse_button_just_released(MouseButton button) const {
        int button_index = static_cast<int>(button);
        if (button_index < 0 || button_index >= static_cast<int>(MouseButton::Count)) {
            return false;
        }

        return mouse_button_states[button_index] == KeyState::Released &&
               (prev_mouse_button_states[button_index] == KeyState::Pressed ||
                prev_mouse_button_states[button_index] == KeyState::Held);
    }

    void InputManager::get_mouse_position(double &x, double &y) const {
        x = mouse_x;
        y = mouse_y;
    }

    void InputManager::get_mouse_movement(double &x, double &y) const {
        x = mouse_x - last_mouse_x;
        y = mouse_y - last_mouse_y;
    }

    void InputManager::get_mouse_scroll(double &x, double &y) const {
        x = scroll_x;
        y = scroll_y;
    }

    int InputManager::register_key_callback(int keyCode, std::function<void(int, KeyState)> callback) {
        if (keyCode < 0 || keyCode >= 512) {
            SC_ERROR("Invalid key code: {} for callback registration", keyCode);
            return -1;
        }

        int callback_id = next_callback_id++;
        key_callbacks[callback_id] = {keyCode, callback};

        SC_DEBUG("Registered callback {} for key {}", callback_id, keyCode);
        return callback_id;
    }

    void InputManager::unregister_key_callback(int id) {
        auto it = key_callbacks.find(id);
        if (it != key_callbacks.end()) {
            SC_DEBUG("Unregistered callback {} for key {}", id, it->second.key_code);
            key_callbacks.erase(it);
        } else {
            SC_WARN("Attempted to unregister non-existent callback: {}", id);
        }
    }

    int InputManager::map_scan_code_to_key(int scanCode) const {
        return SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scanCode), SDL_KMOD_NONE, false);
    }

    std::string InputManager::get_key_name(int keyCode) {
        const char *key_name = SDL_GetKeyName(keyCode);
        return key_name ? std::string(key_name) : "Unknown";
    }
}
