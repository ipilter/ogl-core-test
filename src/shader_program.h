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

  struct attribute_kind
  {
    enum Enum
    {
      vertex
      , color
      , normal
      , uv
      , count
    };
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

  void set_attribute_location(const attribute_kind::Enum, const unsigned location);
  unsigned attribute_location(const attribute_kind::Enum) const;

  void set_need_normal_matrix(const bool v);
  bool need_normal_matrix() const;
  void set_need_height_field(const bool v);
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
  int getUniformLoc(const std::string& name) const;

private:
  unsigned m_program;
  unsigned m_vertex_shader;
  unsigned m_fragment_shader;
  unsigned m_geometry_shader;

  std::map<unsigned, unsigned> m_id_kind_table;
  std::map<attribute_kind::Enum, unsigned> m_attribute_location_table;

  bool m_need_normal_matrix;
  bool m_need_texture;

private:
  static std::unordered_map<unsigned, std::string>& kind_name_table();

private:
  shader_program(const shader_program&) {};
  shader_program& operator = (const shader_program&) {};
};
}
