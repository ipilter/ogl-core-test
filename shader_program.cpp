#include <sstream>

#include "shader_program.h"
#include "opengl.h"

namespace opengl
{
shader_program::shader_program(const std::string& vs, const std::string& fs)
  : m_isValid(true)
{
  m_shaderProgram = glCreateProgram();
  m_isValid &= m_shaderProgram != 0;

  if (m_isValid)
  {
    m_isValid &= AddVertexShader(vs);
    m_isValid &= AddFragmentShader(fs);
  }
  if (m_isValid)
  {
    m_isValid &= Link();
  }
}

shader_program::~shader_program(void)
{
  glDetachShader(m_shaderProgram, m_vertexShaderId);
  glDetachShader(m_shaderProgram, m_fragmentShaderId);
  glDeleteShader(m_vertexShaderId);
  glDeleteShader(m_fragmentShaderId);
  glDeleteProgram(m_shaderProgram);
}

bool shader_program::AddVertexShader(const std::string& src)
{
  m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  if (m_vertexShaderId == 0)
  {
    return false;
  }

  glAttachShader(m_shaderProgram, m_vertexShaderId);
  const char* str[] = { src.c_str() };
  glShaderSource(m_vertexShaderId, 1, str, 0);
  return CompileVertexShader();
}

bool shader_program::AddFragmentShader(const std::string& src)
{
  m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  if (m_fragmentShaderId == 0)
  {
    return false;
  }

  glAttachShader(m_shaderProgram, m_fragmentShaderId);
  const char* str[] = { src.c_str() };
  glCreateShaderProgramv(m_fragmentShaderId, 1, str);
  return CompileFragmentShader();
}

bool shader_program::Link(void)
{
  glLinkProgram(m_shaderProgram);
  GLint linked(-1);
  glGetShaderiv(m_shaderProgram, GL_LINK_STATUS, &linked);

  int InfoLogLength;
  glGetShaderiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::string ProgramErrorMessage(InfoLogLength + 1, ' ');
    glGetProgramInfoLog(m_shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }
  return linked > 0;
}

bool shader_program::CompileVertexShader(void)
{
  glCompileShader(m_vertexShaderId);
  GLint compiled(-1);
  glGetShaderiv(m_vertexShaderId, GL_COMPILE_STATUS, &compiled);
  return compiled > 0;
}

bool shader_program::CompileFragmentShader(void)
{
  bool ret(true);
  glCompileShader(m_fragmentShaderId);
  GLint compiled(-1);
  glGetShaderiv(m_fragmentShaderId, GL_COMPILE_STATUS, &compiled);
  return compiled > 0;
}

void shader_program::bind(void)  const
{
  if (!m_isValid)
  {
    return;
  }
  glUseProgram(m_shaderProgram);
}

bool shader_program::IsValid(void) const
{
  return m_isValid;
}

void shader_program::GetProgramInfoLog(std::string& log) const
{
  const GLsizei maxLenght(1024);
  GLsizei lenghtWritten(0);
  char cstr[maxLenght] = { };
  glGetProgramInfoLog(m_shaderProgram, maxLenght, &lenghtWritten, cstr);
  log.assign(cstr);
}

void shader_program::GetVertexInfoLog(std::string& log) const
{
  const GLsizei maxLenght(1024);
  GLsizei lenghtWritten(0);
  char cstr[maxLenght] = { };
  glGetProgramInfoLog(m_vertexShaderId, maxLenght, &lenghtWritten, cstr);
  log.assign(cstr);
}

void shader_program::GetFragmentInfoLog(std::string& log) const
{
  const GLsizei maxLenght(1024);
  GLsizei lenghtWritten(0);
  char cstr[maxLenght] = { };
  glGetProgramInfoLog(m_fragmentShaderId, maxLenght, &lenghtWritten, cstr);
  log.assign(cstr);
}

int shader_program::getUniformLoc(const std::string& name)
{
  int loc(glGetUniformLocation(m_shaderProgram, name.c_str()));
  if (loc < 0) 
  {
    throw std::runtime_error("Uniform variable could not be reserved.");
  }
  return loc;
}

void shader_program::setUniform1f(const std::string& name, float value0)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform1f(getUniformLoc(name), value0);
}

void shader_program::setUniform2f(const std::string& name, float value0, float value1)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform2f(getUniformLoc(name), value0, value1);
}

void shader_program::setUniform3f(const std::string& name, float value0, float value1, float value2)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform3f(getUniformLoc(name), value0, value1, value2);
}

void shader_program::setUniform4f(const std::string& name, float value0, float value1, float value2, float value3)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform4f(getUniformLoc(name), value0, value1, value2, value3);
}

void shader_program::setUniform1fv(const std::string& name, unsigned count, float* ptr)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform1fv(getUniformLoc(name), count, ptr);
}

void shader_program::setUniform2fv(const std::string& name, unsigned count, float* ptr)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform2fv(getUniformLoc(name), count, ptr);
}

void shader_program::setUniform3fv(const std::string& name, unsigned count, float* ptr)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform3fv(getUniformLoc(name), count, ptr);
}

void shader_program::setUniform4fv(const std::string& name, unsigned count, float* ptr)
{
  if (!m_isValid)
  {
    throw std::runtime_error("Setting uniform variable to an invalid shader_program.");
  }

  glUniform4fv(getUniformLoc(name), count, ptr);
}
};
