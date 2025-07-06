#version 450
#include "postProcessSubpass.frag.glsl"

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 o_color;

void main() {
    o_color = subpassLoad(i_color);
    return;
}
