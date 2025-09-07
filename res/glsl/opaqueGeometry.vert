
#version 450
#include "global.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in uint color;     // 4个8位uint打包为uint32
layout(location = 2) in uint normal;    // 打包的法线数据
layout(location = 3) in mat4 model;

layout(location = 0) out vec4 outputAlbedo;
layout(location = 1) out vec3 outputNormal;

void main(void) {
    // 解包颜色：RGBA打包的uint32
    vec4 unpackedColor = vec4(
        float((color >> 24) & 0xFFu),  // R
        float((color >> 16) & 0xFFu),  // G  
        float((color >>  8) & 0xFFu),  // B
        float((color      ) & 0xFFu)   // A
    ) * (1.0/255.0);

    // 6个主要方向的法线（6位）
    const vec3 normalTable[6] = vec3[6](
        vec3(-1.0, 0.0, 0.0), // 左
        vec3( 1.0, 0.0, 0.0), // 右
        vec3( 0.0,-1.0, 0.0), // 下
        vec3( 0.0, 1.0, 0.0), // 上
        vec3( 0.0, 0.0,-1.0), // 后
        vec3( 0.0, 0.0, 1.0)  // 前
    );
    
    // 找到最大点乘结果的方向（按照你最开始的逻辑）
    vec3 lightDirection = normalize(vec3(0.0, 0.0, 1.0)); // 可以从uniform传入
    uint normalMask = normal & 0x3Fu; // 保证只有6位
    
    float maxDotProduct = -1.0;
    vec3 bestNormal = vec3(0.0, 0.0, 1.0); // 默认
    
    for (int i = 0; i < 6; i++) {
        uint bitSet = (normalMask >> uint(i)) & 1u;
        float dotProduct = dot(normalTable[i], lightDirection);
        float isSet = float(bitSet);
        
        // 如果这个位设置了且点乘更大，则更新
        float isBetter = step(maxDotProduct, dotProduct) * isSet;
        maxDotProduct = mix(maxDotProduct, dotProduct, isBetter);
        bestNormal = mix(bestNormal, normalTable[i], isBetter);
    }

    vec4 worldPosition = model * vec4(position, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    gl_PointSize = globalUniform.pointSizeFactor / -viewPosition.z;
    
    // outputNormal = normalize(mat3(model) * bestNormal);
    outputNormal = vec3(0.0, 0.0, 1.0);
    outputAlbedo = unpackedColor;
}
