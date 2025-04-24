#pragma once
#include "logging.hpp"

class Window;

/**
 * @class Renderer
 * @brief Manages rendering using bgfx.
 *
 * This class handles all rendering operations, including setting up the rendering context,
 * managing render states, and drawing the scene.
 */
class Renderer {
    SC_LOG_GROUP(GRAPHICS::RENDERER);

public:
    Renderer();

    ~Renderer();

    /**
     * @brief Initializes the renderer with the specified window
     * @param window Pointer to the window to render to
     * @param vsync Whether to enable vsync
     * @return True if initialization succeeded, false otherwise
     */
    bool init(Window *window, bool vsync);

    /**
     * @brief Begins a new frame for rendering
     */
    void begin_frame();

    /**
     * @brief Ends the current frame and presents it to the screen
     */
    void end_frame();

    /**
     * @brief Resizes the renderer viewport
     * @param width New viewport width
     * @param height New viewport height
     */
    void resize(int width, int height);

    /**
     * @brief Sets the clear color for the render target
     * @param r Red component (0-1)
     * @param g Green component (0-1)
     * @param b Blue component (0-1)
     * @param a Alpha component (0-1)
     */
    void set_clear_color(float r, float g, float b, float a);

    [[nodiscard]] float get_width() const { return width; }
    [[nodiscard]] float get_height() const { return height; }

private:
    Window *window;
    uint32_t reset_flags;
    uint32_t clear_flags;
    uint32_t width;
    uint32_t height;
    bool vsync;
    bgfx::FrameBufferHandle frame_buffer = BGFX_INVALID_HANDLE;
    float clear_color[4];
    bool initialized;
};
