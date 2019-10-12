#include <sstream>

#include "shader_program.h"

namespace opengl
{

shader_program::shader_program()
  : m_program(glCreateProgram())
  , m_vertex_shader(0)
  , m_geometry_shader(0)
  , m_fragment_shader(0)
  , m_need_normal_matrix(false)
  , m_need_texture(false)
{}

shader_program::~shader_program()
{
  if (m_vertex_shader)
  {
    glDetachShader(m_program, m_vertex_shader);
    glDeleteShader(m_vertex_shader);
  }

  if (m_fragment_shader)
  {
    glDetachShader(m_program, m_fragment_shader);
    glDeleteShader(m_fragment_shader);
  }

  if (m_geometry_shader)
  {
    glDetachShader(m_program, m_geometry_shader);
    glDeleteShader(m_geometry_shader);
  }

  glDeleteProgram(m_program);
}

void shader_program::add_vertex_shader(const std::string& src)
{
  m_vertex_shader = add_shader(GL_VERTEX_SHADER, src);
}

void shader_program::add_geometry_shader(const std::string& src)
{
  m_geometry_shader = add_shader(GL_GEOMETRY_SHADER, src);
}

void shader_program::add_fragment_shader(const std::string& src)
{
  m_fragment_shader = add_shader(GL_FRAGMENT_SHADER, src);
}

unsigned shader_program::add_shader(const unsigned shader_kind , const std::string& src)
{
  const unsigned shader_id = glCreateShader(shader_kind);
  if (shader_id == 0)
  {
    std::stringstream ss;
    ss << "cannot create shader " << shader_kind;
    throw std::runtime_error(ss.str());
  }

  m_id_kind_table[shader_id] = shader_kind;

  glAttachShader(m_program, shader_id);
  const char* str[] = { src.c_str() };
  glShaderSource(shader_id, 1, str, 0);

  compile(shader_id);
  return shader_id;
}

void shader_program::link()
{
  glLinkProgram(m_program);
  GLint linked(GL_FALSE);
  glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE)
  {
    int info_size(0);
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &info_size);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetProgramInfoLog(m_program, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }
    std::stringstream ss;
    ss << "cannot link shader program\n" << msg;
    throw std::runtime_error(ss.str());
  }
}

void shader_program::set_attribute_location(const attribute_kind::Enum attribute_kind, const unsigned location)
{
  m_attribute_location_table[attribute_kind] = location;
}

unsigned shader_program::attribute_location(const attribute_kind::Enum attribute_kind) const
{
  std::map<attribute_kind::Enum, unsigned>::const_iterator it = m_attribute_location_table.find(attribute_kind);
  if (it == m_attribute_location_table.end())
  {
    return invalid_attribute_location();
  }
  return it->second;
}

void shader_program::set_need_normal_matrix(const bool v)
{
  m_need_normal_matrix = v;
}

bool shader_program::need_normal_matrix() const
{
  return m_need_normal_matrix;
}

void shader_program::set_need_texture(const bool v)
{
  m_need_texture = v;
}

bool shader_program::need_texture() const
{
  return m_need_texture;
}

void shader_program::compile(const unsigned shader_id)
{
  glCompileShader(shader_id);

  GLint compiled(GL_FALSE);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE)
  {
    int info_size(0);

    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_size);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetShaderInfoLog(shader_id, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }
    std::stringstream ss;
    ss << "cannot compile " << kind_name_table()[m_id_kind_table[shader_id]] << " (id " << shader_id << ")\n" << msg;
    throw std::runtime_error(ss.str());
  }
}

unsigned shader_program::id() const
{
  return m_program;
}

void shader_program::setUniformMatrix4fv(const std::string& name, const mat4& matrix) const
{
  glUniformMatrix4fv(getUniformLoc(name), 1, GL_FALSE, &matrix[0][0]);
}

int shader_program::getUniformLoc(const std::string& name) const
{
  int loc(glGetUniformLocation(m_program, name.c_str()));
  if (loc < 0)
  {
    check_gl();
    std::stringstream ss;
    ss << "Uniform variable [" << name << "] could not be created.";
    throw std::runtime_error(ss.str());
  }

  return loc;
}

unsigned shader_program::invalid_attribute_location()
{
  return static_cast<unsigned>(-1);
}

void shader_program::setUniform1i(const std::string& name, const unsigned value) const
{
  glUniform1i(getUniformLoc(name), value);
}

void shader_program::setUniform1f(const std::string& name, const float value) const
{
  glUniform1f(getUniformLoc(name), value);
}

void shader_program::setUniform2f(const std::string& name, const float value0, const float value1) const
{
  glUniform2f(getUniformLoc(name), value0, value1);
}

void shader_program::setUniform3f(const std::string& name, const float value0, const float value1, const float value2) const
{
  glUniform3f(getUniformLoc(name), value0, value1, value2);
}

void shader_program::setUniform4f(const std::string& name, const float value0, const float value1, const float value2, const float value3) const
{
  glUniform4f(getUniformLoc(name), value0, value1, value2, value3);
}

void shader_program::setUniform1fv(const std::string& name, const unsigned count, float* ptr) const
{
  glUniform1fv(getUniformLoc(name), count, ptr);
}

void shader_program::setUniform2fv(const std::string& name, const unsigned count, float* ptr) const
{
  glUniform2fv(getUniformLoc(name), count, ptr);
}

void shader_program::setUniform3fv(const std::string& name, const unsigned count, float* ptr) const
{
  glUniform3fv(getUniformLoc(name), count, ptr);
}

void shader_program::setUniform4fv(const std::string& name, const unsigned count, float* ptr) const
{
  glUniform4fv(getUniformLoc(name), count, ptr);
}

std::unordered_map<unsigned, std::string>& shader_program::kind_name_table()
{
  static std::unordered_map<unsigned, std::string> s_kind_name_table;
  if (s_kind_name_table.empty())
  {
    s_kind_name_table[GL_VERTEX_SHADER] = "vertex shader";
    s_kind_name_table[GL_GEOMETRY_SHADER] = "geometry shader";
    s_kind_name_table[GL_FRAGMENT_SHADER] = "fragment shader";
  }
  return s_kind_name_table;
}
}
