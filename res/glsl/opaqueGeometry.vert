#version 450
precision highp float;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec4 inputColor;
layout(location = 2) in vec3 inputNormal;
layout(location = 3) in mat4 inputModel;

layout(location = 0) out vec4 outputAlbedo;
layout(location = 1) out vec3 outputNormal;

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
    vec4 worldPosition = inputModel * vec4(inputPosition, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    gl_PointSize = globalUniform.pointSizeFactor / -viewPosition.z;
    outputNormal = normalize(mat3(inputModel) * inputNormal);
    outputAlbedo = inputColor;
}
