$input a_position, a_normal
$output v_color0

#include <bgfx_shader.sh>
#include <bgfx_compute.sh>
#include "shaderlib.sh"

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    
    vec3 normal = normalize(mul(u_model[0], vec4(a_normal, 0.0)).xyz);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float ndotl = max(dot(normal, lightDir), 0.1);
    
    v_color0 = vec4(ndotl, ndotl, ndotl, 1.0);
}