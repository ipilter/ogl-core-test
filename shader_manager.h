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

  void create (const std::string& name, const std::string&, const std::string&);
  void remove (const std::string& name);
  void remove_all ();

  void bind (const std::string& name);
  void unbind ();

  shader_program* get_program (const std::string& name);

private:
  std::map<std::string, shader_program*> m_shaderPrograms;

  shader_manager(const shader_manager&) = delete;
  shader_manager& operator = (const shader_manager&) = delete;
};

};
