#pragma once

#include <map>
#include <string>

#include "shader_program.h"

namespace opengl
{
class shader_manager
{
public:
  shader_manager();
  ~shader_manager();

  shader_program& add(const std::string& name);
  void remove(const std::string& name);
  void clear();

  const shader_program& get(const std::string& name) const;

private:
  std::map<std::string, shader_program::ptr> m_shader_programs;

  shader_manager(const shader_manager&) {};
  shader_manager& operator = (const shader_manager&) {};
};
};
