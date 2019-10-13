#pragma once

#include <vector>
#include <memory>

#include "shader_program.h"
#include "types.h"

namespace opengl
{
class mesh
{
public:
  using ptr = std::shared_ptr<mesh>;

public:
  mesh(const std::vector<vec3>& vertices, const std::vector<unsigned>& indices, const unsigned primitive_type);
  ~mesh();

  void render(const shader_program& shader_program, const mat4& view_projection);

  void add_colors(const std::vector<vec3>& buffer);
  void add_normals(const std::vector<vec3>& buffer);
  void add_uvs(const std::vector<vec2>& buffer);
  void set_texture(const unsigned texture_id);
  void set_transformation(const mat4& m);
  const mat4& get_transformation() const;

private:
  void enable_vertex_attribute(const unsigned attribute_location, const unsigned buffer_id, const unsigned size);

  template<typename T>
  void add_buffer(const std::vector<T>& buffer, const unsigned buffer_kind, unsigned& id, int usage = GL_STATIC_DRAW)
  {
    glBindVertexArray(m_vertex_array_id);
    glGenBuffers(1, &id);
    glBindBuffer(buffer_kind, id);
    glBufferData(buffer_kind, sizeof(T) * buffer.size(), &buffer[0], usage);
    glBindBuffer(buffer_kind, 0);
    glBindVertexArray(0);
  }

private:
  unsigned m_primitive_type;
  unsigned m_primitive_count;

  mat4 m_transformation;

  // TODO array instead these..
  unsigned m_vertex_array_id;
  unsigned m_vertex_buffer_id;
  unsigned m_color_buffer_id;
  unsigned m_normal_buffer_id;
  unsigned m_uv_buffer_id;
  unsigned m_index_buffer_id;

  unsigned m_texture_id;
};
}
