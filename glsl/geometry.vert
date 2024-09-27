#version 450
precision highp float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec3 o_normal;

layout(binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    vec3 camera_world_pos;
} globalUniform;

layout(binding = 1) uniform ObjectUniform
{
    mat4 model;
} objectUniform;

void main(void)
{
    vec4 worldPosition = objectUniform.model * vec4(i_position, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    // vec3 normalizedViewDirection = normalize(globalUniform.camera_world_pos - worldPosition.xyz);
    // vec3 baseVectors[6] = {
    //     vec3(1,0,0),
    //     vec3(-1,0,0),
    //     vec3(0,1,0),
    //     vec3(0,-1,0),
    //     vec3(0,0,1),
    //     vec3(0,0,-1),
    // };
    
    // float maxDot = -1;
    // for (int i = 0; i < 6; ++i) {
    //     float dotResult = dot(normalizedViewDirection, baseVectors[i]);
    //     if(dotResult > maxDot){
    //         o_normal = baseVectors[i];
    //         maxDot = dotResult;
    //     }
    // }
    float z = (gl_Position.z / gl_Position.w + 1) * 0.5;
    o_albedo = vec4(gl_Position.z, gl_Position.w, 1, 1);
    // o_normal = vec3(step(max(viewDirection.y, viewDirection.z), viewDirection.x), step(max(viewDirection.x, viewDirection.z), viewDirection.y), step(max(viewDirection.x, viewDirection.y), viewDirection.z));
    gl_PointSize = 1 / (z);
}