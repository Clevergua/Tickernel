#version 450
precision highp float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in mat4 i_model;

layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec3 o_normal;

layout(binding = 0) uniform GlobalUniform {
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float pointSizeFactor;
    float time;
} globalUniform;

float random(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float waveFunction(vec3 position, float time) {
    float wave = sin(position.x * 10.0 + globalUniform.time) * 0.1 +
        cos(position.y * 10.0 + globalUniform.time) * 0.1;
    wave += (random(position.xy * 10.0) - 0.5) * 0.02;
    return wave;
}

void main(void) {
    float wave = waveFunction(i_position, globalUniform.time);
    vec3 modifiedPosition = vec3(i_position.x, i_position.y, i_position.z + wave);

    vec4 worldPosition = i_model * vec4(modifiedPosition, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    o_albedo = i_color;
    o_albedo.a = 0.5;
    o_normal = i_normal;
    float scale = (i_model[0][0] + i_model[1][1] + i_model[2][2]) / 3.0;
    gl_PointSize = scale * globalUniform.pointSizeFactor / -viewPosition.z;
}
