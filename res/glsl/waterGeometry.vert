#version 450
#include "global.glsl"
layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in mat4 i_model;

layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec3 o_normal;

float random(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float waveFunction(vec3 position, float time)
{
    float wave = sin(position.x * 10.0 + globalUniform.time) * 0.1 +
        cos(position.y * 10.0 + globalUniform.time) * 0.1;
    wave += (random(position.xy * 10.0) - 0.5) * 0.02;
    return wave;
}

void main(void)
{
    float wave = waveFunction(i_position, globalUniform.time * 0.01);
    vec3 modifiedPosition = vec3(i_position.x, i_position.y, i_position.z + wave);

    vec4 worldPosition = i_model * vec4(modifiedPosition, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    gl_PointSize = globalUniform.pointSizeFactor / -viewPosition.z;
    o_normal = normalize(mat3(i_model) * i_normal);
    o_albedo = i_color;
}
