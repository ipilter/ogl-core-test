// vert ppd
#version 330 core

layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_uv;

uniform mat4 model_view_projection_matrix;

uniform sampler2D height_map;

void main()
{
  //float height = texture(height_map, vertex_uv).x * 0.0001;

  vec3 p = vertex_position_modelspace;
  gl_Position = vec4(p, 1.0);
}
