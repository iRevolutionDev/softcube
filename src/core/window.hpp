#pragma once

/**
 * @class Window
 * @brief Manages the application window using GLFW.
 *
 * This class is responsible for creating and managing the application window,
 * handling window events, and providing access to the window handle for rendering.
 */
class Window {
public:
    Window();

    ~Window();

    /**
     * @brief Initializes the window with the specified settings
     * @param width Initial window width
     * @param height Initial window height
     * @param title Window title
     * @param fullscreen Whether to create a fullscreen window
     * @return True if initialization succeeded, false otherwise
     */
    bool init(int width, int height, const std::string &title, bool fullscreen);

    /**
     * @brief Updates the window state and processes events
     */
    void update();

    /**
     * @brief Checks if the window should be closed
     * @return True if the window should close, false otherwise
     */
    bool should_close() const;

    /**
     * @brief Gets the GLFW window handle
     * @return Pointer to the GLFW window
     */
    void *get_native_handle() const { return window; }

    /**
     * @brief Gets the current window width
     * @return Window width in pixels
     */
    int get_width() const { return width; }

    /**
     * @brief Gets the current window height
     * @return Window height in pixels
     */
    int get_height() const { return height; }

    /**
     * @brief Sets a new window title
     * @param title New window title
     */
    void set_title(const std::string &title);

private:
    SDL_Window *window;
    int width;
    int height;
    bool fullscreen;
    std::string title;
};
