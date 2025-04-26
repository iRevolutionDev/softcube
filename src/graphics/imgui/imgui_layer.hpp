#pragma once

#include "core/logging.hpp"
#include "imgui_freetype.h"

#define VIEW_ID (255 & 0xff)

class ImGuiLayer {
    SC_LOG_GROUP(GRAPHICS::IMGUI_LAYER);

public:
    ImGuiLayer();

    ~ImGuiLayer();

    void init();

    void shutdown() const;

    void reset(uint16_t width, uint16_t height);

    void render(ImDrawData *draw_data) const;

    void new_frame();

    void update_font_texture();

    /**
     * @brief Loads a font using FreeType and adds it to ImGui
     * 
     * @param font_path Path to the font file
     * @param size_pixels Font size in pixels
     * @param font_cfg Optional font configuration
     * @param glyph_ranges Optional glyph ranges
     * @return ImFont* Pointer to the loaded font
     */
    ImFont *add_font(const char *font_path, float size_pixels,
                     const ImFontConfig *font_cfg = nullptr,
                     const ImWchar *glyph_ranges = nullptr) const;

    /**
     * @brief Loads default font with FreeType
     * 
     * @param size_pixels Font size in pixels
     * @param font_cfg Optional font configuration
     * @return ImFont* Pointer to the loaded font
     */
    ImFont *add_default_font(float size_pixels = 13.0f,
                             const ImFontConfig *font_cfg = nullptr) const;

private:
    bgfx::VertexLayout m_vertex_layout{};
    bgfx::ProgramHandle m_program{};
    bgfx::TextureHandle m_font_texture{};
    bgfx::UniformHandle m_texture_uniform{};

    int m_freetype_flags = ImGuiFreeTypeBuilderFlags_LightHinting |
                           ImGuiFreeTypeBuilderFlags_ForceAutoHint;
};

