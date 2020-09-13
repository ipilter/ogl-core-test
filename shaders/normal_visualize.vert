// vert nv
#version 330 core

layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_uv;

uniform sampler2D height_field;

out vec2 v_uv;

void main()
{
  float height = texture(height_field, vertex_uv).x;
  v_uv = vertex_uv;
  gl_Position = vec4(vertex_position_modelspace + vec3(0.0, 0.0, height), 1.0);
}
