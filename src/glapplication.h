#pragma once

#include "opengl.h"
#include "mesh.h"
#include "types.h"
#include "io.h"
#include "shader_manager.h"
#include "height_field.h"
#include "camera.h"

namespace opengl
{
class GLApplication
{
public:
  struct draw_mode
  {
    enum Enum
    {
      shaded
      , shaded_wireframe
      , wireframe
      , count
    };
  };

  struct settings
  {
    settings(const vec3& cp, const vec3& co, draw_mode::Enum dm, bool ra, bool rn)
      : camera_position(cp)
      , camera_orientation(co)
      , m_draw_mode(dm)
      , m_render_axis(ra)
      , m_render_normals(rn)
    {}

    vec3 camera_position;
    vec3 camera_orientation;
    draw_mode::Enum m_draw_mode;
    bool m_render_axis;
    bool m_render_normals;
  };

public:
  static GLApplication& instance();

public:
  void init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version);
  void run();
  void render();
  void request_update();

private:
  void create_scene();
  void destroy_scene() noexcept;
  void parse_settings(const std::string& path);
  void save_settings(const std::string& path);

private:
  // camera
  camera m_camera;

  // scene
  mesh::ptr m_axis;
  std::vector<mesh::ptr> m_meshes;
  shader_manager m_shader_manager;
  terrain::height_field::ptr m_height_field;
  unsigned m_height_field_texture_id;

  vec3 m_background_color;

  bool m_mouse_left_down;
  vec2 m_mouse_position;
  float m_mouse_sensitivity;
  float m_keyboard_speed;
  float m_keyboard_step;
  settings m_settings;

private:
  static const vec3& world_up();

private:
  static void display_callback();
  static void reshape_callback(int w, int h);
  static void keyboard_callback(unsigned char character, int x, int y);
  static void mouse_button_callback(int button, int state, int x, int y);
  static void mouse_move_callback(int x, int y);

private:
  GLApplication();
  ~GLApplication();
};
}
