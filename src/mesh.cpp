#pragma once

#include <vector>
#include "opengl.h"
#include "types.h"
#include "mesh.h"

namespace opengl
{
mesh::mesh(const std::vector<vec3>& vertices, const std::vector<unsigned>& indices, const unsigned primitive_type)
  : m_primitive_type(primitive_type)
  , m_primitive_count(0)
  , m_transformation(1)
  , m_vertex_array_id(0)
  , m_vertex_buffer_id(0)
  , m_color_buffer_id(0)
  , m_normal_buffer_id(0)
  , m_uv_buffer_id(0)
  , m_index_buffer_id(0)
  , m_texture_id(0)
{
  glGenVertexArrays(1, &m_vertex_array_id);

  add_buffer(vertices, GL_ARRAY_BUFFER, m_vertex_buffer_id);
  add_buffer(indices, GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id);

  m_primitive_count = static_cast<unsigned>(indices.size());
}

mesh::~mesh()
{
  glDeleteVertexArrays(1, &m_vertex_array_id);
  glDeleteBuffers(1, &m_vertex_buffer_id);
  glDeleteBuffers(1, &m_index_buffer_id);

  if (m_color_buffer_id)
  {
    glDeleteBuffers(1, &m_color_buffer_id);
  }

  if (m_normal_buffer_id)
  {
    glDeleteBuffers(1, &m_normal_buffer_id);
  }

  if (m_uv_buffer_id)
  {
    glDeleteBuffers(1, &m_uv_buffer_id);
  }
}

void mesh::add_colors(const std::vector<vec3>& colors)
{
  add_buffer(colors, GL_ARRAY_BUFFER, m_color_buffer_id);
}

void mesh::add_normals(const std::vector<vec3>& normals)
{
  add_buffer(normals, GL_ARRAY_BUFFER, m_normal_buffer_id);
}

void mesh::add_uvs(const std::vector<vec2>& uvs)
{
  add_buffer(uvs, GL_ARRAY_BUFFER, m_uv_buffer_id);
}

void mesh::set_texture(const unsigned texture_id)
{
  m_texture_id = texture_id;
}

void mesh::set_transformation(const mat4& m)
{
  m_transformation = m;
}

const mat4& mesh::get_transformation() const
{
  return m_transformation;
}

void mesh::render(const shader_program& shader_program, const mat4& view_projection)
{
  glBindVertexArray(m_vertex_array_id);

  const unsigned buff[] = { m_vertex_buffer_id, m_color_buffer_id, m_normal_buffer_id, m_uv_buffer_id }; // buffers
  const unsigned buffer_data_size[] = { 3, 3, 3, 2 }; // buffer element sizes

  // vertex is a must
  enable_vertex_attribute(shader_program.attribute_location(shader_program::attribute_kind::vertex), buff[0], buffer_data_size[0]);

  // additional vertex data
  for(unsigned kind = shader_program::attribute_kind::color; kind < shader_program::attribute_kind::count; ++kind)
  { 
    const unsigned attribute_location = shader_program.attribute_location(shader_program::attribute_kind::Enum(kind));
    if (buff[kind] && attribute_location != shader_program::invalid_attribute_location())
    {
      enable_vertex_attribute(attribute_location, buff[kind], buffer_data_size[kind]);
    }
  }

  glUseProgram(shader_program.id());
  shader_program.setUniformMatrix4fv("model_view_projection_matrix", view_projection * m_transformation);

  if (shader_program.need_normal_matrix())
  {
    shader_program.setUniformMatrix4fv("normal_matrix", glm::transpose(glm::inverse(m_transformation)));
  }

  if (m_texture_id && shader_program.need_texture())
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    shader_program.setUniform1i("height_map", 0); // TODO: 0 texture slot is used for the texture, store it instead
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id);
  glDrawElements(m_primitive_type, static_cast<GLsizei>(m_primitive_count), GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}

void mesh::enable_vertex_attribute(const unsigned attribute_location, const unsigned buffer_id, const unsigned size)
{
  glEnableVertexAttribArray(attribute_location);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glVertexAttribPointer(
    attribute_location,               // attribute location must match the layout in the shader
    size,                             // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );
}

}
