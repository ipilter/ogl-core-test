// frag ppd
#version 330 core

// calculate the normal in geom shader and pass it here

uniform sampler2D height_field;
uniform vec3 light_position;

in vec3 vertex_color;
in vec3 vertex_normal;
in vec3 vertex;

out vec4 color;

void main()
{
  color = vec4(0.0, 0.0, 0.0, 1.0);

  vec3 N = normalize (vertex_normal);
  vec3 L = normalize (light_position - vertex);
  vec3 E = normalize(-vertex);
  vec3 R = reflect (-L, N);

  float lambertTerm = dot (N, L);

  vec4 light_diffuse = vec4(1.0);
  float light_specular;
  vec4 material_diffuse = vec4(1.0);
  vec4 material_specular = vec4(0.3);
  float material_shininess = 0.3;

  if(lambertTerm > 0.0)
  {
    color += light_diffuse * material_diffuse * lambertTerm;
    float specular = pow( max(dot(R, E), 0.0), material_shininess);
    color += light_specular * material_specular * specular;
  }
}
