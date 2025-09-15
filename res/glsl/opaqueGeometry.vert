
#version 450
#include "global.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in uint color;
layout(location = 2) in uint normal;
layout(location = 3) in mat4 model;

layout(location = 0) out vec4 outputAlbedo;
layout(location = 1) out vec3 outputNormal;

void main(void) {
    vec4 unpackedColor = unpackUnorm4x8(color);

    const vec3 normalTable[6] = vec3[6](vec3(-1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0));

    vec3 lightDirection = normalize(vec3(0.0, 0.0, 1.0));
    uint normalMask = normal & 0x3Fu;

    float maxDotProduct = -1.0;
    vec3 bestNormal = vec3(0.0, 0.0, 1.0); // 默认

    for(int i = 0; i < 6; i++) {
        uint bitSet = (normalMask >> uint(i)) & 1u;
        if(bitSet == 0u)
            continue;
        vec3 worldNormal = normalize(mat3(model) * normalTable[i]);
        float dotProduct = dot(worldNormal, lightDirection);
        if(dotProduct > maxDotProduct) {
            maxDotProduct = dotProduct;
            bestNormal = worldNormal;
        }
    }

    vec4 worldPosition = model * vec4(position, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    gl_PointSize = globalUniform.pointSizeFactor / -viewPosition.z;

    outputNormal = bestNormal;
    outputAlbedo = unpackedColor;
}
