// vert ppd
#version 330 core

layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_uv;

uniform mat4 model_view_projection_matrix;

uniform sampler2D height_map;

out shader_data
{
  vec2 uv;
} vs_out;

void main()
{
  float height = texture(height_map, vertex_uv).x;
  gl_Position = model_view_projection_matrix * vec4(vertex_position_modelspace + vec3(0.0, height, 0.0), 1.0);
  vs_out.uv = vertex_uv;
}
