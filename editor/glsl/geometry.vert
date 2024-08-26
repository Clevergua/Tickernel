#version 320 es
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(set = 0, binding = 1) uniform GlobalUniform {
  mat4 model;
  mat4 view_proj;
  vec3 camera_position;
}
global_uniform;

layout(location = 0) out vec4 o_pos;
layout(location = 1) out vec2 o_uv;
layout(location = 2) out vec3 o_normal;

void main(void) {
  o_pos = global_uniform.model * vec4(position, 1.0);
  gl_Position = global_uniform.view_proj * o_pos;
  gl_PointSize = 1.0 / gl_Position.z;
}
