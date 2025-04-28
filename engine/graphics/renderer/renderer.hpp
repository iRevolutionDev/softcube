#pragma once
#include "core/logging.hpp"
#include "graphics/layers/imgui_layer.hpp"
#include "graphics/layers/editor_layer.hpp"

namespace softcube {
    class Window;
    class EcsManager;

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
        static void set_clear_color(float r, float g, float b, float a);

        /**
         * @brief Begins a new ImGui frame
         *
         * This method initializes an ImGui frame for UI rendering.
         * Should be called after begin_frame() and before any ImGui commands.
         */
        void begin_imgui();

        /**
         * @brief Ends and renders the current ImGui frame
         *
         * This method finalizes the ImGui frame and renders all UI elements.
         * Should be called after all ImGui commands and before end_frame().
         */
        void end_imgui();

        /**
         * @brief Gets the ImGui layer used for UI rendering
         * @return Pointer to the ImGuiLayer instance
         */
        [[nodiscard]] ImGuiLayer *get_imgui_layer() const { return imgui_layer; }

        [[nodiscard]] float get_width() const { return width; }
        [[nodiscard]] float get_height() const { return height; }

        /**
         * @brief Initialize the editor layer
         * @param ecs_manager Pointer to the ECS manager
         */
        void init_editor(EcsManager *ecs_manager);

        /**
         * @brief Render the editor UI
         */
        void render_editor();

        /**
         * @brief Get the editor layer
         * @return Pointer to the EditorLayer instance
         */
        [[nodiscard]] EditorLayer *get_editor_layer() const { return editor_layer; }

        /**
         * @brief Set if the editor is enabled
         * @param enabled Whether the editor should be enabled
         */
        void set_editor_enabled(bool enabled) { editor_enabled = enabled; }

        /**
         * @brief Check if the editor is enabled
         * @return True if the editor is enabled, false otherwise
         */
        [[nodiscard]] bool is_editor_enabled() const { return editor_enabled; }

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

        // ImGui layer for UI rendering
        ImGuiLayer *imgui_layer = nullptr;

        EditorLayer *editor_layer = nullptr;
        bool editor_enabled = true;
    };
}
