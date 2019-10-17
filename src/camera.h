#pragma once

#include "types.h"

namespace opengl
{
class camera
{
public:
  camera();

  const vec3& position() const;
  void set_position(const vec3& position);

  const vec3& orientation() const;
  void set_orientation(const vec3& orientation);

  void set_window_size(const uvec2& size);
  const uvec2& window_size() const;

  const mat4& view_matrix() const;
  const mat4& projection_matrix() const;

  void update_view();
  void update_projection();

private:
  vec3 m_position;
  vec3 m_orientation; // roll pitch yaw
  uvec2 m_window_size;
  mat4 m_projection;
  mat4 m_view;
};
}
