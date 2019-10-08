// vert nv
#version 450 core

layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec3 vertex_normal;

out vec3 normal;

void main()
{
  gl_Position = vec4(vertex_position_modelspace, 1.0);
  normal = vertex_normal;
}
