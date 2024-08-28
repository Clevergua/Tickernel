#version 450
precision highp float;

layout(location = 1) in vec4 i_color;
layout(location = 0) out vec4 o_albedo;
layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
}
ubo;

void main(void)
{
    o_albedo = i_color;
}
