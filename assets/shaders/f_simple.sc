$input v_color0

#include <bgfx_shader.sh>
#include <bgfx_compute.sh>
#include "shaderlib.sh"

void main() {
    gl_FragColor = v_color0 * vec4(0, 1.0, 1.0, 1.0);
}