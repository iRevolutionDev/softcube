#pragma once
#include <unordered_map>
#include <array>
#include <functional>
#include <string>

class Window;

/**
 * @class InputManager
 * @brief Manages all input from keyboard, mouse, and controllers.
 *
 * This class handles input polling, key/button state tracking, and input mapping.
 * It provides a high-level interface for querying input states and registering callbacks.
 */
class InputManager {
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
    bool init(Window* window);

    /**
     * @brief Updates input states and processes input events
     * @param deltaTime Time since the last update
     */
    void update(double deltaTime);

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
     * @brief Gets the current mouse position
     * @param x Output parameter for x coordinate
     * @param y Output parameter for y coordinate
     */
    void get_mouse_position(double& x, double& y) const;

    /**
     * @brief Gets the mouse movement since the last frame
     * @param x Output parameter for x movement
     * @param y Output parameter for y movement
     */
    void get_mouse_movement(double& x, double& y) const;

    /**
     * @brief Gets the mouse scroll wheel delta
     * @param x Output parameter for horizontal scroll
     * @param y Output parameter for vertical scroll
     */
    void get_mouse_scroll(double& x, double& y) const;

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

private:
    Window* window;
    
    // Key and mouse states
    std::array<KeyState, 512> current_key_states;
    std::array<KeyState, 512> previous_key_states;
    std::array<bool, static_cast<size_t>(MouseButton::Count)> mouse_button_states;
    std::array<bool, static_cast<size_t>(MouseButton::Count)> prev_mouse_button_states;
    
    // Mouse position and movement
    double mouse_x;
    double mouse_y;
    double last_mouse_x;
    double last_mouse_y;
    double scroll_x;
    double scroll_y;
    
    // Callbacks
    struct KeyCallback {
        int key_code;
        std::function<void(int, KeyState)> callback;
    };
    std::unordered_map<int, KeyCallback> key_callbacks;
    int next_callback_id;

    // GLFW callback functions (to be registered with the window)
    static void keyCallback(void* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(void* window, int button, int action, int mods);
    static void cursorPosCallback(void* window, double xpos, double ypos);
    static void scrollCallback(void* window, double xoffset, double yoffset);
};
