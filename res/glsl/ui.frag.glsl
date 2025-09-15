#include "tickernel.glsl"

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uiTexture;

void main() {
    vec4 texColor = texture(uiTexture, inUV);
    outColor = texColor * inColor;
}
