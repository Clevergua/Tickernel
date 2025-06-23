#version 450
layout(location = 0) out vec2 outUV;
layout(binding = 5) uniform fFF {
    mat4 view;
    int width;
    int height;
} bufffffffer;

layout(binding = 3) uniform GlobalUniform {
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
void main() {
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0 - 1.0, 0.0, 1.0);
}
