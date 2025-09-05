#include "tickernel.glsl"
layout(set = GLOBAL_DESCRIPTOR_SET, binding = 0) uniform GlobalUniform {
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float pointSizeFactor;
    float time;
    int frameCount;
    float near;
    float far;
    float fov;
} globalUniform;

