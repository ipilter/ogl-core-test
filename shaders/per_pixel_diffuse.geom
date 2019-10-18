// geom wf
#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 vertex_color;
out vec3 vertex_normal;
out vec3 vertex;

uniform mat4 model_view_projection_matrix;
uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;

void main()
{
  for (int i = 0; i < 3; i++)
  {
    vec3 p = gl_in[i].gl_Position.xyz;
    vec3 pprev = gl_in[i == 0                  ? gl_in.length() - 1 : i - 1].gl_Position.xyz;
    vec3 pnext = gl_in[i == gl_in.length() - 1 ?                  0 : i + 1].gl_Position.xyz;
    
    vec3 normal = (normal_matrix  * vec4(normalize((cross(pnext-p, pprev-p))), 1.0)).xyz;
    
    gl_Position = model_view_projection_matrix * vec4(p, 1.0);

    vertex = (model_view_matrix * gl_in[i].gl_Position).xyz;

    vertex_color = vec3(1.0, 0.0, 0.0);
    vertex_normal = normal;

    EmitVertex();
  }
  EndPrimitive();
}
