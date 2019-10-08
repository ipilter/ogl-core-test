#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 normal[];

uniform mat4 MVP;

out vec3 fragment_color;

void main()
{
  for(int i = 0; i < gl_in.length(); ++i)
  {
    fragment_color = vec3(1);
    gl_Position = MVP * gl_in[i].gl_Position;
    EmitVertex();

    fragment_color = vec3(1);
    gl_Position = MVP * vec4(gl_in[i].gl_Position.xyz + normal[i], 1);
    EmitVertex();

    EndPrimitive();
  }
}
