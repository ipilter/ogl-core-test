#pragma once

#include <vector>
#include "opengl.h"
#include "types.h"
#include "mesh.h"

namespace opengl
{
mesh::mesh()
{}

mesh::~mesh()
{
  glDeleteVertexArrays(1, &m_vertex_array);

  glDeleteBuffers(1, &m_vertex_buffer);
  glDeleteBuffers(1, &m_index_buffer);

  if (m_color_buffer)
  {
    glDeleteBuffers(1, &m_color_buffer);
  }
  if (m_normal_buffer)
  {
    glDeleteBuffers(1, &m_normal_buffer);
  }
}

void mesh::render(const shader_program& shader_program, const mat4& projection, const mat4& view)
{
  glBindVertexArray(m_vertex_array);

  unsigned attrib_id(0);

  glEnableVertexAttribArray(attrib_id);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glVertexAttribPointer(
    attrib_id,          // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );
  ++attrib_id;

  if (m_color_buffer)
  {
    glEnableVertexAttribArray(attrib_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
    glVertexAttribPointer(
      attrib_id,                        // attribute. No particular reason for 1, but must match the layout in the shader.
      3,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
    );
    ++attrib_id;
  }

  if (m_normal_buffer)
  {
    glEnableVertexAttribArray(attrib_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glVertexAttribPointer(
      attrib_id,                        // attribute. No particular reason for 1, but must match the layout in the shader.
      3,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
    );
    ++attrib_id;
  }

  glUseProgram(shader_program.id());
  mat4 mvp = projection * view/* * Model*/;
  unsigned MatrixID = glGetUniformLocation(shader_program.id(), "MVP");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
  glDrawElements(m_primitive_type, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glDisableVertexAttribArray(0);
}

void mesh::create(const std::vector<vec3>& vertices
                  , const std::vector<vec3> colors
                  , const std::vector<vec3> normals
                  , std::vector<unsigned> indices)
{
  // create cpu side
  m_vertices = vertices;
  m_colors = colors;
  m_normals = normals;
  m_indices = indices;

  // upload to gpu
  glGenVertexArrays(1, &m_vertex_array);
  glBindVertexArray(m_vertex_array);

  glGenBuffers(1, &m_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  opengl::check_gl();
}

void mesh::create(const std::vector<vec3>& vertices
                  , const std::vector<vec3> colors
                  , std::vector<unsigned> indices)
{
  // create cpu side
  m_vertices = vertices;
  m_colors = colors;
  m_indices = indices;

  // upload to gpu
  glGenVertexArrays(1, &m_vertex_array);
  glBindVertexArray(m_vertex_array);

  glGenBuffers(1, &m_vertex_buffer);
  glGenBuffers(1, &m_index_buffer);
  glGenBuffers(1, &m_color_buffer);

  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  opengl::check_gl();
}

void mesh::set_primitive_type(unsigned type)
{
  m_primitive_type = type;
}

}