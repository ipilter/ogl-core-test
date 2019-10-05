#pragma once

#include <vector>

#include "opengl.h"
#include "types.h"
#include "mesh.h"

mesh::mesh()
{}

mesh::~mesh()
{
  glDeleteVertexArrays(1, &m_vertex_array);
  glDeleteBuffers(1, &m_vertex_buffer);
  glDeleteBuffers(1, &m_index_buffer);
  glDeleteBuffers(1, &m_color_buffer);
}

void mesh::render(unsigned shader_program, const mat4& projection, const mat4& view)
{
  glBindVertexArray(m_vertex_array);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  glUseProgram(shader_program);
  mat4 mvp = projection * view/* * Model*/;
  unsigned MatrixID = glGetUniformLocation(shader_program, "MVP");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
  glDrawElements(m_primitive_type, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glDisableVertexAttribArray(0);
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
