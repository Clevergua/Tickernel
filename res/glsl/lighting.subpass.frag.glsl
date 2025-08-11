#include "tickernel.glsl"

layout(set = SUBPASS_DESCRIPTOR_SET, input_attachment_index = 1, binding = 0) uniform subpassInput i_depth;
layout(set = SUBPASS_DESCRIPTOR_SET, input_attachment_index = 2, binding = 1) uniform subpassInput i_albedo;
layout(set = SUBPASS_DESCRIPTOR_SET, input_attachment_index = 3, binding = 2) uniform subpassInput i_normal;

#define POINT_LIGHT_COUNT 256
struct DirectionalLight {
    vec4 color;
    vec3 direction;
};
struct PointLight {
    vec4 color;
    vec3 position;
    float range;
};
layout(set = SUBPASS_DESCRIPTOR_SET, binding = 3) uniform LightsUniform {
    DirectionalLight directionalLight;
    int pointLightCount;
    PointLight pointLights[POINT_LIGHT_COUNT];
} lightsUniform;



