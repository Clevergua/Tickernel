#version 450
precision highp float;

layout(location = 0) in vec4 i_albedo;
layout(location = 1) in vec3 i_normal;
layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec4 o_normal;

void main(void)
{
    o_albedo = i_albedo;
     // Transform normals from [-1, 1] to [0, 1]
    o_normal = vec4(0.5 * i_normal + 0.5, 1.0);
}