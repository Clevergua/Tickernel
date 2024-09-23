#version 450
precision highp float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

layout(location = 0) out vec4 o_albedo;

layout(binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 proj;
} globalUniform;

layout(binding = 1) uniform ObjectUniform
{
    mat4 model;
} objectUniform;

void main(void)
{
    gl_Position = globalUniform.proj * globalUniform.view * objectUniform.model * vec4(i_position, 1);
    o_albedo = i_color;
    float z = (0.5f * (gl_Position.z / gl_Position.w) + 0.5f);
    gl_PointSize = z * 5.0f;
}