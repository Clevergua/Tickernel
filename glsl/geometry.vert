#version 450
precision highp float;

layout(location = 0) in vec4 i_position;
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
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(i_position.xyz, 0);
    o_albedo = i_color;
    gl_PointSize = gl_Position.z;
}