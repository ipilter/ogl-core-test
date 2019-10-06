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
{
  remove_all();
}

void shader_manager::create(const std::string& name
                            , const std::string& vsFileName
                            , const std::string& fsFileName)
{
  if (m_shader_programs.find(name) != m_shader_programs.end())
  {
    std::stringstream ss;
    ss << "Shader program with name [";
    ss << name;
    ss << "] already exist!";
    throw std::runtime_error(ss.str());
  }

  std::string vs, fs;
  io::load_src(vsFileName, vs);
  io::load_src(fsFileName, fs);
  shader_program::ptr program(new shader_program(vs, fs));

  if (program == nullptr)
  {
    throw std::runtime_error("out of memory");
  }

  m_shader_programs[name] = program;
}

void shader_manager::remove_all (void)
{
  m_shader_programs.clear ();
}

void shader_manager::remove(const std::string& name)
{
  if (m_shader_programs.find(name) != m_shader_programs.end())
  {
    m_shader_programs.erase(name);
  }
}

const shader_program& shader_manager::get(const std::string& name) const
{
  const auto it = m_shader_programs.find(name);
  if (it == m_shader_programs.end())
  {
    std::stringstream ss;
    ss << "Shader program with name [";
    ss << name;
    ss << "] does not exist!";
    throw std::runtime_error (ss.str ());
  }

  return *it->second;
}
};
