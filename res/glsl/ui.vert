#version 450

#include "tickernel.glsl"

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in uint color;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    outUV = uv;
    outColor = unpackUnorm4x8(color);
}
