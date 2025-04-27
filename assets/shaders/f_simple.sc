$input v_color0

#include <bgfx_shader.sh>
#include <bgfx_compute.sh>
#include "shaderlib.sh"

uniform vec4 u_color;

void main() {
    gl_FragColor = u_color;
}