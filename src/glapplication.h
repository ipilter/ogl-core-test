#pragma once

#include "opengl.h"
#include "mesh.h"
#include "types.h"
#include "io.h"
#include "shader_manager.h"

// ideas from
// https://open.gl/content/code/c2_triangle_elements.txt

class GLApplication
{
public:
  static GLApplication& instance();
  static void terminate();

public:
  void init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version);
  void run();
  void render();
  void request_update();
  void update_projection();
  void flip_axis();

private:
  void create_scene();
  void destroy_scene() noexcept;

private:
  // camera
  uvec2 m_window_size;
  mat4 m_projection;
  mat4 m_view;

  bool m_render_axis;

  std::vector<opengl::mesh::ptr> m_meshes;

  opengl::shader_manager m_shader_manager;

private:
  static void display_callback();
  static void reshape_callback(int w, int h);
  static void keyboard_callback(unsigned char character, int x, int y);

private:
  GLApplication();
  ~GLApplication();
};
