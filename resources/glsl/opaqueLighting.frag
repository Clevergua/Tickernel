#version 450
precision highp float;

layout(input_attachment_index = 0, binding = 0) uniform subpassInput i_depth;
layout(input_attachment_index = 1, binding = 1) uniform subpassInput i_albedo;
layout(input_attachment_index = 2, binding = 2) uniform subpassInput i_normal;
layout(binding = 3) uniform GlobalUniform {
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float pointSizeFactor;
} globalUniform;

struct DirectionalLight {
    vec4 color;
    vec3 direction;
};
struct PointLight {
    vec4 color;
    vec3 position;
    float range;
};
layout(binding = 4) uniform LightsUniform {
    DirectionalLight directionalLight;
    int pointLightCount;
    PointLight pointLights[256];
} lightsUniform;

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 o_color;

void main() {

    vec4 clip = vec4(in_uv * 2.0 - 1.0, subpassLoad(i_depth).x, 1.0);
    vec4 world_w = globalUniform.inv_view_proj * clip;
    vec3 position = world_w.xyz / world_w.w;
    // o_color = vec4(position, 1);
    // return;
    vec3 normal = normalize(subpassLoad(i_normal).xyz - 0.5 * 2);
    float ndl = max(dot(normal, normalize(lightsUniform.directionalLight.direction)), 0.0);
    float halfLambert = ndl * 0.5 + 0.5;
    vec4 albedo = subpassLoad(i_albedo);

    vec3 o_rgb = albedo.rgb * lightsUniform.directionalLight.color.rgb * lightsUniform.directionalLight.color.a * halfLambert;
    for(int i = 0; i < lightsUniform.pointLightCount; i++) {
        PointLight pointLight = lightsUniform.pointLights[i];
        vec3 direction = position - pointLight.position;
        float distance = length(direction);
        float attenuation = clamp(1.0 - (distance / pointLight.range), 0.0, 1.0);
        float attenuationStep = step(distance, pointLight.range);
        direction = normalize(direction);
        ndl = max(dot(normal, direction), 0.0);
        halfLambert = ndl * 0.5 + 0.5;
        o_rgb += albedo.rgb * pointLight.color.rgb * pointLight.color.a * halfLambert * attenuation * attenuationStep;
    }
    o_color = vec4(o_rgb, 1.0);
    return;
}
