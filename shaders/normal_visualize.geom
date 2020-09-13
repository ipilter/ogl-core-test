// geom nv
#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

out vec3 vertex_color;

uniform mat4 model_view_projection_matrix;
uniform mat4 normal_matrix;

in vec2 v_uv[];

void main()
{
  for(int i = 0; i < gl_in.length(); ++i)
  {
    vec3 p = gl_in[i].gl_Position.xyz;
    vec3 n = vec3(0,1,0);    
    n = (normal_matrix * vec4(n, 0.0)).xyz;

    gl_Position = model_view_projection_matrix * vec4(p, 1);
    vertex_color = (normal_matrix * vec4(n, 0.0)).xyz; //(normal_matrix * vec4(v_uv[i].xy, 0.0, 0.0)).xyz
    EmitVertex();

    gl_Position =  model_view_projection_matrix * vec4(p + n, 1);
    vertex_color = (normal_matrix * vec4(n, 0.0)).xyz;
    EmitVertex();

    EndPrimitive();
  }
}
