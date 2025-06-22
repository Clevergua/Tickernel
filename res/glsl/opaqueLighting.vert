#version 450
layout(location = 0) out vec2 outUV;
layout(binding = 5) uniform GlobalUniform {
    mat4 view;
    int width;
    int height;
} bufffffffer;
void main() {
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0 - 1.0, 0.0, 1.0);
}
