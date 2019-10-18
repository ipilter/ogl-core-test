#include <stdexcept>
#include <sstream>

#include "opengl.h"
#include "shader_manager.h"
#include "io.h"

namespace opengl
{
shader_manager::shader_manager()
{ }

shader_manager::~shader_manager()
{ }

shader_program& shader_manager::add(const std::string& name)
{
  if (m_shader_programs.find(name) != m_shader_programs.end())
  {
    std::stringstream ss;
    ss << "Shader program [" << name << "] already exist!";
    throw std::runtime_error(ss.str());
  }

  shader_program::ptr program(new shader_program(name));
  if (program == nullptr)
  {
    throw std::runtime_error("out of memory");
  }

  m_shader_programs[name] = program;
  return *program;
}

void shader_manager::clear(void)
{
  m_shader_programs.clear ();
}

void shader_manager::remove(const std::string& name)
{
  const auto it = m_shader_programs.find(name);
  if (it == m_shader_programs.end())
  {
    std::stringstream ss;
    ss << "Shader program [" << name << "] does not exist!";
    throw std::runtime_error (ss.str ());
  }
  m_shader_programs.erase(it);
}

const shader_program& shader_manager::get(const std::string& name) const
{
  const auto it = m_shader_programs.find(name);
  if (it == m_shader_programs.end())
  {
    std::stringstream ss;
    ss << "Shader program [" << name << "] does not exist!";
    throw std::runtime_error (ss.str ());
  }
  return *it->second;
}
}
