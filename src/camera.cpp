#include "camera.h"

namespace opengl
{
camera::camera()
  : m_position(0.0f)
  , m_window_size(1, 1)
  , m_projection(1)
  , m_view(1)
{ }

void camera::set_position(const vec3& position)
{
  m_position = position;
  update_view();
}

const vec3& camera::position() const
{
  return m_position;
}

void camera::set_orientation(const vec3& orientation)
{
  m_orientation = orientation;
  update_view();
}

const vec3& camera::orientation() const
{
  return m_orientation;
}

void camera::set_window_size(const uvec2& size)
{
  m_window_size = size;
}

const uvec2& camera::window_size() const
{
  return m_window_size;
}

const mat4& camera::view_matrix() const
{
  return m_view;
}

const mat4& camera::projection_matrix() const
{
  return m_projection;
}

void camera::update_view()
{
  glm::mat4 matPitch = glm::mat4(1.0f);
  glm::mat4 matYaw = glm::mat4(1.0f);

  matPitch = glm::rotate(matPitch, glm::radians(m_orientation[1]), glm::vec3(1.0f, 0.0f, 0.0f));
  matYaw = glm::rotate(matYaw, glm::radians(m_orientation[2]), glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 rotate = matPitch * matYaw;

  glm::mat4 translate = glm::mat4(1.0f);
  translate = glm::translate(translate, -m_position);

  m_view = rotate * translate;
}

void camera::update_projection()
{
  m_projection = glm::perspectiveFov(glm::radians(60.0), static_cast<double>(m_window_size.x), static_cast<double>(m_window_size.y), 0.01, 1000.0);
}
}
