#include "shader_utils.hpp"

namespace shaders {
    bgfx::ShaderHandle create_shader(const std::string &shader_path, const char *name) {
        const auto current_path = std::filesystem::current_path();
        const auto path = current_path / "assets" / "shaders" / shader_path;

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open shader file: " + path.string());
        }

        const std::vector shader((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

        const auto *mem = bgfx::copy(shader.data(), shader.size());
        const auto handle = createShader(mem);
        setName(handle, name);

        return handle;
    }
}
