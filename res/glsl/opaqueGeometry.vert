#version 450
precision highp float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in mat4 i_model;

layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec3 o_normal;

layout(set = 0, binding = 0) uniform GlobalUniform {
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float pointSizeFactor;
    float time;
    int frameCount;
    float near;
    float far;
    float fov;
    int width;
    int height;
} globalUniform;

void main(void) {
    vec4 worldPosition = i_model * vec4(i_position, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    gl_PointSize = globalUniform.pointSizeFactor / -viewPosition.z;
    o_normal = normalize(mat3(i_model) * i_normal);
    o_albedo = i_color;
}
