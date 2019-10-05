#include <sstream>

#include "shader_program.h"
#include "opengl.h"

namespace opengl
{
shader_program::shader_program(const std::string& vs
                               , const std::string& fs)
{
  m_shader_program = glCreateProgram();
  m_vertex_shader = add_shader(GL_VERTEX_SHADER, vs);
  m_fragment_shader = add_shader(GL_FRAGMENT_SHADER, fs);

  link();

  opengl::check_gl();
}

shader_program::~shader_program(void)
{
  glDetachShader(m_shader_program, m_vertex_shader);
  glDetachShader(m_shader_program, m_fragment_shader);
  glDeleteShader(m_vertex_shader);
  glDeleteShader(m_fragment_shader);
  glDeleteProgram(m_shader_program);
}

unsigned shader_program::add_shader(unsigned shader_kind
                                    , const std::string& src)
{
  unsigned shader_id = glCreateShader(shader_kind);
  if (shader_id == 0)
  {
    std::stringstream ss;
    ss << "cannot create shader " << shader_kind;
    throw std::runtime_error(ss.str());
  }

  glAttachShader(m_shader_program, shader_id);
  const char* str[] = { src.c_str() };
  glShaderSource(shader_id, 1, str, 0);

  opengl::check_gl();

  compile(shader_id);
  return shader_id;
}

void shader_program::link()
{
  glLinkProgram(m_shader_program);
  
  GLint linked(0);
  glGetProgramiv(m_shader_program, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE)
  {
    GLint status = GL_FALSE;
    int info_size(0);

    glGetShaderiv(m_shader_program, GL_INFO_LOG_LENGTH, &info_size);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetShaderInfoLog(m_shader_program, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }
    throw std::runtime_error(std::string("cannot link shader program: ") + msg);
  }
}

void shader_program::compile(unsigned shader_id)
{
  glCompileShader(shader_id);

  GLint compiled(0);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE)
  {
    GLint status = GL_FALSE;
    int info_size(0);

    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &compiled);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetShaderInfoLog(shader_id, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }
    throw std::runtime_error(std::string("cannot compile vertex shader: ") + msg);
  }
}

unsigned shader_program::id() const
{
  return m_shader_program;
}

//void shader_program::GetProgramInfoLog(std::string& log) const
//{
//  const GLsizei maxLenght(1024);
//  GLsizei lenghtWritten(0);
//  char cstr[maxLenght] = { };
//  glGetProgramInfoLog(m_shader_program, maxLenght, &lenghtWritten, cstr);
//  log.assign(cstr);
//}
//
//void shader_program::GetVertexInfoLog(std::string& log) const
//{
//  const GLsizei maxLenght(1024);
//  GLsizei lenghtWritten(0);
//  char cstr[maxLenght] = { };
//  glGetProgramInfoLog(m_vertex_shader, maxLenght, &lenghtWritten, cstr);
//  log.assign(cstr);
//}
//
//void shader_program::GetFragmentInfoLog(std::string& log) const
//{
//  const GLsizei maxLenght(1024);
//  GLsizei lenghtWritten(0);
//  char cstr[maxLenght] = { };
//  glGetProgramInfoLog(m_fragment_shader, maxLenght, &lenghtWritten, cstr);
//  log.assign(cstr);
//}

//int shader_program::getUniformLoc(const std::string& name)
//{
//  int loc(glGetUniformLocation(m_shader_program, name.c_str()));
//  if (loc < 0) 
//  {
//    throw std::runtime_error("Uniform variable could not be reserved.");
//  }
//  return loc;
//}
//
//void shader_program::setUniform1f(const std::string& name, float value0)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform1f(getUniformLoc(name), value0);
//}
//
//void shader_program::setUniform2f(const std::string& name, float value0, float value1)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform2f(getUniformLoc(name), value0, value1);
//}
//
//void shader_program::setUniform3f(const std::string& name, float value0, float value1, float value2)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform3f(getUniformLoc(name), value0, value1, value2);
//}
//
//void shader_program::setUniform4f(const std::string& name, float value0, float value1, float value2, float value3)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform4f(getUniformLoc(name), value0, value1, value2, value3);
//}
//
//void shader_program::setUniform1fv(const std::string& name, unsigned count, float* ptr)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform1fv(getUniformLoc(name), count, ptr);
//}
//
//void shader_program::setUniform2fv(const std::string& name, unsigned count, float* ptr)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform2fv(getUniformLoc(name), count, ptr);
//}
//
//void shader_program::setUniform3fv(const std::string& name, unsigned count, float* ptr)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform3fv(getUniformLoc(name), count, ptr);
//}
//
//void shader_program::setUniform4fv(const std::string& name, unsigned count, float* ptr)
//{
//  if (!m_is_valid)
//  {
//    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
//  }
//
//  glUniform4fv(getUniformLoc(name), count, ptr);
//}
};
