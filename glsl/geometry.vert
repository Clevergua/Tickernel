#version 450
precision highp float;

layout(location = 0) in vec4 i_position;
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
    gl_Position = globalUniform.proj * globalUniform.view * objectUniform.model * vec4(i_position.xyz, 0);
    o_albedo = i_color;         
    gl_PointSize = gl_Position.z;
}