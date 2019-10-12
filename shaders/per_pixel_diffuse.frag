// frag ppd
#version 330 core


// calculate the normal in geom shader and pass it here

uniform sampler2D height_map;

in shader_data
{
  vec2 uv;
} vs_out;

out vec4 color;

void main()
{
  float height = texture(height_map, vs_out.uv).x;
  color = vec4(height, height, height, 1.0);
}
