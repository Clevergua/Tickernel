#version 450
precision highp float;
layout(binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    vec3 camera_world_pos;
} globalUniform;

layout(input_attachment_index = 0, binding = 1) uniform subpassInput i_depth;
layout(input_attachment_index = 1, binding = 2) uniform subpassInput i_albedo;
layout(input_attachment_index = 2, binding = 3) uniform subpassInput i_normal;

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 o_color;

void main()
{
    vec4 clip = vec4(in_uv * 2.0 - 1.0, subpassLoad(i_depth).x, 1.0);
    vec4 world_w = globalUniform.inv_view_proj * clip;
    vec3 position = world_w.xyz / world_w.w;

    vec3 normal = subpassLoad(i_normal).xyz;

    o_color = subpassLoad(i_albedo);
    return;

    vec3 lightDirection = vec3(0, 0, -1);
    vec4 lightColor = vec4(1, 1, 1, 1);
    float ndl = max(dot(normalize(normal), normalize(lightDirection)), 0.0);
    float halfLambert = ndl * 0.5 + 0.5;
    vec4 albedo = subpassLoad(i_albedo);
    o_color = albedo * lightColor * halfLambert;
    return;
}