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

  void create(const std::string& name
              , const std::string&
              , const std::string&);
  void remove(const std::string& name);
  void remove_all();

  const shader_program& get(const std::string& name) const;

private:
  std::map<std::string, shader_program::ptr> m_shader_programs;

  shader_manager(const shader_manager&) = delete;
  shader_manager& operator = (const shader_manager&) = delete;
};
};
