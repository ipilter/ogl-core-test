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
  typedef std::shared_ptr<mesh> ptr;

public:
  mesh(const unsigned primitive_type);
  ~mesh();

  void render(const shader_program& shader_program, const mat4& view_projection);

  void add_vertices(const std::vector<vec3>& buffer);
  void add_colors(const std::vector<vec3>& buffer);
  void add_normals(const std::vector<vec3>& buffer);
  void add_uvs(const std::vector<vec2>& buffer);
  void add_indices(const std::vector<unsigned>& buffer);
  void set_texture(const unsigned texture_id);
  void set_transform(const mat4& m);
  const mat4& get_transform() const;

private:
  void enable_vertex_attribute(const unsigned attribute_location, const unsigned buffer_id, const unsigned size);

  template<typename T>
  void add_buffer(const std::vector<T>& buffer, const unsigned buffer_kind, unsigned& id)
  {
    glBindVertexArray(m_vertex_array_id);
    glGenBuffers(1, &id);
    glBindBuffer(buffer_kind, id);
    glBufferData(buffer_kind, sizeof(T) * buffer.size(), &buffer[0], GL_STATIC_DRAW);
    glBindBuffer(buffer_kind, 0);
    glBindVertexArray(0);
  }

private:
  unsigned m_primitive_type;
  unsigned m_primitive_count;

  mat4 m_model_transform;

  unsigned m_vertex_array_id;
  unsigned m_vertex_buffer_id;
  unsigned m_color_buffer_id;
  unsigned m_normal_buffer_id;
  unsigned m_uv_buffer_id;
  unsigned m_index_buffer_id;

  unsigned m_texture_id;
};
}
