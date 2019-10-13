// geom wf
#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

out vec3 vertex_color;

uniform mat4 model_view_projection_matrix;

void main()
{
  for(int i = 0; i < gl_in.length(); ++i)
  {
    vec3 p = gl_in[i].gl_Position.xyz;
    vec3 pnext = gl_in[i == gl_in.length() - 1 ? 0 : i + 1].gl_Position.xyz;

    gl_Position = model_view_projection_matrix * vec4(p, 1);
    vertex_color = vec3(1.0, 1.0, 1.0);
    EmitVertex();

    gl_Position = model_view_projection_matrix * vec4(pnext, 1);
    vertex_color = vec3(1.0, 1.0, 1.0);
    EmitVertex();

    EndPrimitive();
  }
}
