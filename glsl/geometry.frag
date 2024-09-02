#version 450
precision highp float;

layout(location = 0) in vec4 i_albedo;
layout(location = 0) out vec4 o_albedo;

void main(void)
{
    o_albedo = i_albedo;
}