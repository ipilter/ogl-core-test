#pragma once

#include <string>
#include <memory>
#include <map>
#include <unordered_map>

#include "types.h"
#include "opengl.h"

namespace opengl
{
class shader_program
{
public:
  using ptr = std::shared_ptr<shader_program>;

  enum AttributeKind
  {
    vertex_attribute
    , color_attribute
    , normal_attribute
    , uv_attribute
  };

  static unsigned invalid_attribute_location();

public:
  shader_program();
  ~shader_program();

  unsigned id() const;

  void add_vertex_shader(const std::string& src);
  void add_geometry_shader(const std::string& src);
  void add_fragment_shader(const std::string& src);
  void link();

  void set_attribute_loc(const AttributeKind, const unsigned location);
  unsigned get_attribute_loc(const AttributeKind) const;

  void set_need_normal_matrix(const bool v);
  bool need_normal_matrix() const;
  void set_need_texture(const bool v);
  bool need_texture() const;

  void setUniform1i(const std::string& name, const unsigned value) const;

  void setUniform1f(const std::string& name, const float value) const;
  void setUniform2f(const std::string& name, const float value0, const float value1) const;
  void setUniform3f(const std::string& name, const float value0, const float value1, const float value2) const;
  void setUniform4f(const std::string& name, const float value0, const float value1, const float value2, const float value3) const;

  void setUniform1fv(const std::string& name, const unsigned count, float* ptr) const;
  void setUniform2fv(const std::string& name, const unsigned count, float* ptr) const;
  void setUniform3fv(const std::string& name, const unsigned count, float* ptr) const;
  void setUniform4fv(const std::string& name, const unsigned count, float* ptr) const;

  void setUniformMatrix4fv(const std::string& name, const mat4& matrix) const;

private:
  unsigned add_shader(const unsigned shader_kind, const std::string& src);

  void compile(const unsigned shader_id);

private:
  int getUniformLoc(const std::string& name) const;

  unsigned m_program;
  unsigned m_vertex_shader;
  unsigned m_fragment_shader;
  unsigned m_geometry_shader;

  std::map<unsigned, unsigned> m_id_kind_table;
  std::map<AttributeKind, unsigned> m_attribute_table;

  bool m_need_normal_matrix;
  bool m_need_texture;

  static std::unordered_map<unsigned, std::string>& kind_name_table()
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

  shader_program(const shader_program&) {};
  shader_program& operator = (const shader_program&) {};
};
}
