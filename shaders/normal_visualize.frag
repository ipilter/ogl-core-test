// frag nv
#version 330 core

in vec3 vertex_color;

out vec3 color;

void main()
{
  color = abs(vertex_color);
}
