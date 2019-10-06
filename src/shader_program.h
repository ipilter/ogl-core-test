#pragma once

#include <string>
#include <memory>
#include <map>
#include <unordered_map>

#include "opengl.h"

namespace opengl
{
class shader_program
{
public:
  using ptr = std::shared_ptr<shader_program>;

public:
  shader_program(const std::string& vs, const std::string& fs);
  shader_program(const std::string& vs, const std::string& gs, const std::string& fs);
  ~shader_program();

  unsigned add_shader(const unsigned shader_kind
                      , const std::string&);

  unsigned id() const;

private:
  void compile(unsigned shader_id);
  void link();

  //void GetProgramInfoLog(std::string&) const;
  //void GetVertexInfoLog(std::string&) const;
  //void GetFragmentInfoLog(std::string&) const;
  //void setUniform1f(const std::string& name, float value0);
  //void setUniform2f(const std::string& name, float value0, float value1);
  //void setUniform3f(const std::string& name, float value0, float value1, float value2);
  //void setUniform4f(const std::string& name, float value0, float value1, float value2, float value3);

  //void setUniform1fv(const std::string& name, unsigned count, float* ptr);
  //void setUniform2fv(const std::string& name, unsigned count, float* ptr);
  //void setUniform3fv(const std::string& name, unsigned count, float* ptr);
  //void setUniform4fv(const std::string& name, unsigned count, float* ptr);

private:
  //int getUniformLoc(const std::string& name);

  unsigned m_program;
  unsigned m_vertex_shader;
  unsigned m_fragment_shader;
  unsigned m_geometry_shader;

  std::map<unsigned, unsigned> m_id_kind_table;

  static std::unordered_map<unsigned, std::string>& kind_name_table()
  {
    static std::unordered_map<unsigned, std::string> s_kind_name_table{
      { GL_VERTEX_SHADER, "vertex shader"},
      { GL_GEOMETRY_SHADER, "geometry shader"},
      { GL_FRAGMENT_SHADER, "fragment shader"}
    };
    return s_kind_name_table;
  }

  shader_program(const shader_program&) = delete;
  shader_program& operator = (const shader_program&) = delete;
};
};

