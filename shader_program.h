#pragma once

#include <string>
#include <memory>

namespace opengl
{
class shader_program
{
public:
  using ptr = std::shared_ptr<shader_program>;

public:
  shader_program(const std::string&, const std::string&);
  ~shader_program();

  unsigned add_shader(const unsigned shader_kind
                      , const std::string&);

  void compile(unsigned shader_id);
  void link();

  unsigned id() const;

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

  unsigned m_shader_program;
  unsigned m_vertex_shader;
  unsigned m_fragment_shader;

  shader_program(const shader_program&) = delete;
  shader_program& operator = (const shader_program&) = delete;
};
};
