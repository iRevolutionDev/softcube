#pragma once

namespace shaders {
    bgfx::ShaderHandle create_shader(const std::string &shader_path, const char *name);
}