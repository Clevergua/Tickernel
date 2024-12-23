#version 450
precision highp float;

layout(input_attachment_index = 0, binding = 0) uniform subpassInput i_color;

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 o_color;

void main() {
    o_color = subpassLoad(i_color);
    return;
}
