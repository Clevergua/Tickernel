#version 450

#include "tickernel.glsl"

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in uint inColor;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    outUV = inUV;
    outColor = unpackUnorm4x8(inColor);
}
