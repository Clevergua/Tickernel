#version 450
precision highp float;

// #ifdef HAS_BASE_COLOR_TEXTURE
// layout (set=0, binding=0) uniform sampler2D base_color_texture;
// #endif

layout (location = 0) in vec4 in_pos;
layout (location = 1) in vec4 in_color;

layout (location = 0) out vec4 o_albedo;
layout (location = 1) out vec4 o_normal;

layout(set = 0, binding = 1) uniform GlobalUniform {
    mat4 model;
    mat4 view_proj;
    vec3 camera_position;
} global_uniform;

// layout(push_constant, std430) uniform PBRMaterialUniform {
//     vec4 base_color_factor;
//     float metallic_factor;
//     float roughness_factor;
// } pbr_material_uniform;

void main(void)
{
    o_albedo = in_color;
}
