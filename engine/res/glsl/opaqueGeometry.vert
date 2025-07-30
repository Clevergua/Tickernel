
#version 450
#include "global.glsl"

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec4 inputColor;
layout(location = 2) in vec3 inputNormal;
layout(location = 3) in mat4 inputModel;

layout(location = 0) out vec4 outputAlbedo;
layout(location = 1) out vec3 outputNormal;

void main(void) {
    vec4 worldPosition = inputModel * vec4(inputPosition, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    gl_PointSize = globalUniform.pointSizeFactor / -viewPosition.z;
    outputNormal = normalize(mat3(inputModel) * inputNormal);
    outputAlbedo = inputColor;
}
