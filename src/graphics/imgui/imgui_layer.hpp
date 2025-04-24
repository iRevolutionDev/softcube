#pragma once

#include "core/logging.hpp"

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

private:
    bgfx::VertexLayout m_vertex_layout{};
    bgfx::ProgramHandle m_program{};
    bgfx::TextureHandle m_font_texture{};
    bgfx::UniformHandle m_texture_uniform{};
};

