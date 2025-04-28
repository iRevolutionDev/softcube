#pragma once
#include "core/common.hpp"
#include "core/logging.hpp"

namespace softcube {
    class Window;

    /**
     * @class InputManager
     * @brief Manages all input from keyboard, mouse, and controllers.
     *
     * This class handles input polling, key/button state tracking, and input mapping.
     * It provides a high-level interface for querying input states and registering callbacks.
     */
    class InputManager {
        SC_LOG_GROUP(CORE::INPUT_MANAGER);

    public:
        enum class KeyState {
            Released = 0,
            Pressed = 1,
            Held = 2
        };

        enum class MouseButton {
            Left = 0,
            Right = 1,
            Middle = 2,
            Button4 = 3,
            Button5 = 4,
            Count
        };

        InputManager();

        ~InputManager();

        /**
         * @brief Initializes the input manager
         * @param window Pointer to the window for input capture
         * @return True if initialization succeeded, false otherwise
         */
        bool init(Window *window);

        /**
         * @brief Updates input states and processes input events
         */
        void update();

        /**
         * @brief Checks if a key is currently pressed
         * @param keyCode The key code to check
         * @return True if the key is pressed, false otherwise
         */
        bool is_key_pressed(int keyCode) const;

        /**
         * @brief Checks if a key was just pressed this frame
         * @param keyCode The key code to check
         * @return True if the key was just pressed, false otherwise
         */
        bool is_key_just_pressed(int keyCode) const;

        /**
         * @brief Checks if a key was just released this frame
         * @param keyCode The key code to check
         * @return True if the key was just released, false otherwise
         */
        bool is_key_just_released(int keyCode) const;

        /**
         * @brief Checks if a mouse button is currently pressed
         * @param button The mouse button to check
         * @return True if the button is pressed, false otherwise
         */
        bool is_mouse_button_pressed(MouseButton button) const;

        /**
         * @brief Checks if a mouse button was just pressed this frame
         * @param button The mouse button to check
         * @return True if the button was just pressed, false otherwise
         */
        bool is_mouse_button_just_pressed(MouseButton button) const;

        /**
         * @brief Checks if a mouse button was just released this frame
         * @param button The mouse button to check
         * @return True if the button was just released, false otherwise
         */
        bool is_mouse_button_just_released(MouseButton button) const;

        /**
         * @brief Gets the current mouse position
         * @param x Output parameter for x coordinate
         * @param y Output parameter for y coordinate
         */
        void get_mouse_position(double &x, double &y) const;

        /**
         * @brief Gets the mouse movement since the last frame
         * @param x Output parameter for x movement
         * @param y Output parameter for y movement
         */
        void get_mouse_movement(double &x, double &y) const;

        /**
         * @brief Gets the mouse scroll wheel delta
         * @param x Output parameter for horizontal scroll
         * @param y Output parameter for vertical scroll
         */
        void get_mouse_scroll(double &x, double &y) const;

        /**
         * @brief Registers a callback function for a key event
         * @param keyCode The key code to register for
         * @param callback The function to call when the key is pressed/released
         * @return An ID that can be used to unregister the callback
         */
        int register_key_callback(int keyCode, std::function<void(int, KeyState)> callback);

        /**
         * @brief Unregisters a previously registered key callback
         * @param id The ID of the callback to unregister
         */
        void unregister_key_callback(int id);

        /**
         * @brief Maps a scancode to a keycode for certain special keys
         * @param scanCode The SDL scan code to convert
         * @return The corresponding keycode
         */        int map_scan_code_to_key(int scanCode) const;
        
        /**
         * @brief Checks if a scancode key is currently pressed
         * @param scanCode The scancode to check
         * @return True if the key with that scancode is pressed, false otherwise
         */
        bool is_scancode_pressed(int scanCode) const;

        bool is_scancode_just_pressed(int scanCode) const;

        /**
         * @brief Gets the name of a key for display purposes
         * @param keyCode The key code to get the name for
         * @return A string representation of the key
         */
        static std::string get_key_name(int keyCode);

        /**
         * @brief Sets relative mouse mode, which captures the mouse and makes the cursor invisible
         * @param enabled Whether to enable or disable relative mouse mode
         */
        void set_relative_mouse_mode(bool enabled);

        /**
         * @brief Checks if relative mouse mode is currently enabled
         * @return True if relative mouse mode is enabled, false otherwise
         */
        bool is_relative_mouse_mode() const;

    private:
        Window *window;

        // Key and mouse states
        std::array<KeyState, 512> current_key_states;
        std::array<KeyState, 512> previous_key_states;
        std::array<KeyState, static_cast<size_t>(MouseButton::Count)> mouse_button_states;
        std::array<KeyState, static_cast<size_t>(MouseButton::Count)> prev_mouse_button_states;

        // Mouse position and movement
        double mouse_x{};
        double mouse_y{};
        double last_mouse_x{};
        double last_mouse_y{};
        double scroll_x{};
        double scroll_y{};
        bool relative_mouse_mode{false};

        // Callbacks
        struct KeyCallback {
            int key_code;
            std::function<void(int, KeyState)> callback;
        };
        
        std::unordered_map<int, KeyCallback> key_callbacks;
        int next_callback_id;
    };
}
