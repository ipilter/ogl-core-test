#pragma once

#include <vector>
#include <memory>
#include "types.h"

class mesh
{
public:
  using ptr = std::shared_ptr<mesh>;

public:
  mesh();
  ~mesh();

  void render(unsigned shader_program
              , const mat4& projection
              , const mat4& view);

  void create(const std::vector<vec3>& vertices
              , const std::vector<vec3> colors
              , std::vector<unsigned> indices);

  void set_primitive_type(unsigned type);

private:
  unsigned m_primitive_type;

  // axis cpu side data
  std::vector<vec3> m_vertices;
  std::vector<vec3> m_colors;
  std::vector<unsigned> m_indices;

  // axis gpu side data
  unsigned m_vertex_array;
  unsigned m_vertex_buffer;
  unsigned m_index_buffer;
  unsigned m_color_buffer;
};
