#version 320 es
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(set = 0, binding = 1) uniform GlobalUniform
{
  mat4 model;
  mat4 view_proj;
  vec3 camera_position;
}
global_uniform;

layout(location = 0) out vec4 o_position;
layout(location = 1) out vec4 o_color;

void main(void)
{
  o_color = vec4(0.53, 0.9, 0.23, 0.43);
  o_position = global_uniform.model * vec4(position, 1.0);
  o_color = color;
  gl_Position = global_uniform.view_proj * o_position;
  gl_PointSize = 1.0 / gl_Position.z;
}
