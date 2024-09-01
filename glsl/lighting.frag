#version 450
precision highp float;

layout(input_attachment_index = 0, binding = 0) uniform subpassInput i_depth;
layout(input_attachment_index = 1, binding = 1) uniform subpassInput i_albedo;

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 o_color;

void main()
{
    vec4 clip = vec4(in_uv * 2.0 - 1.0, subpassLoad(i_depth).x, 1.0);
    // highp vec4 world_w = global_uniform.inv_view_proj * clip;
    // highp vec3 position = world_w.xyz / world_w.w;
    vec4 albedo = subpassLoad(i_albedo);
    o_color = albedo;

}