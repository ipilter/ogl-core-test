#pragma once

#include "opengl.h"
#include "mesh.h"
#include "types.h"
#include "io.h"
#include "shader_manager.h"

namespace opengl
{
class GLApplication
{
public:
  static GLApplication& instance();

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

public:
  void init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version);
  void run();
  void render();
  void request_update();
  void update_projection();

private:
  void create_scene();
  void destroy_scene() noexcept;

private:
  // camera
  uvec2 m_window_size;
  mat4 m_projection;
  mat4 m_view;

  bool m_render_axis;
  draw_mode::Enum m_draw_mode;
  bool m_render_normals;

  std::vector<mesh::ptr> m_meshes;

  shader_manager m_shader_manager;

  unsigned m_height_map_texture_id;

  static const vec3& world_up();

private:
  static void display_callback();
  static void reshape_callback(int w, int h);
  static void keyboard_callback(unsigned char character, int x, int y);

private:
  GLApplication();
  ~GLApplication();
};
}
