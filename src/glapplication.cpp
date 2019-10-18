#pragma once

#include "types.h"
#include "io.h"
#include "glapplication.h"
#include "range.h"
#include "range_mapper.h"

namespace opengl
{
GLApplication& GLApplication::instance()
{
  static GLApplication app;
  return app;
}

GLApplication::GLApplication()
  : m_render_axis(true)
  , m_draw_mode(draw_mode::shaded)
  , m_render_normals(false)
  , m_background_color(0)
  , m_mouse_left_down(false)
  , m_mouse_position(0.0)
  , m_mouse_sensitivity(0.3f)
  , m_keyboard_speed(0.3f)
{}

GLApplication::~GLApplication()
{}

void GLApplication::init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version)
{
  if (argc < 1)
  {
    throw std::runtime_error("missing parameter");
  }

  m_camera.set_window_size(window_size);

  glutInit(&argc, argv);
  glutInitContextVersion(opengl_version.x, opengl_version.y);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(m_camera.window_size().x, m_camera.window_size().y);
  glutCreateWindow("gl");

  if (gl3wInit())
  {
    throw std::runtime_error("failed to initialize OpenGL");
  }

  create_scene();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // glut callbacks
  glutReshapeFunc(reshape_callback);
  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keyboard_callback);
  glutMouseFunc(mouse_button_callback);
  glutMotionFunc(mouse_move_callback);
}

void GLApplication::run()
{
  glutMainLoop();
}

void GLApplication::create_scene()
{
  m_background_color = vec3(0.15, 0.15, 0.15);

  // height field
  {
    const unsigned w(10), h(10);
    std::vector<float> height_field_pixels(w * h, 0.0f);
    for (auto j = 0; j < h; ++j)
    {
      for (auto i = 0; i < w; ++i)
      {
        height_field_pixels[i + j * w] = 0.5f * sin((static_cast<float>(j) / h) * 20.0f) + 0.5f * cos((static_cast<float>(i) / w) * 20.0f);
      }
    }

    m_height_field.reset(new terrain::height_field(w, h, vec2(1.05, 1.05), &height_field_pixels[0]));
  }

  // camera
  m_camera.set_position(vec3(-5.0f, 5.0f, 5.0f));
  m_camera.set_orientation(vec3(0.0f, 20.0f, 90.0f));

  // shaders
  {
    {
      std::string vs_code, fs_code;
      io::load_src("shaders\\simple_color.vert", vs_code);
      io::load_src("shaders\\simple_color.frag", fs_code);

      shader_program& prog = m_shader_manager.add("simple_color");
      prog.add_vertex_shader(vs_code);
      prog.add_fragment_shader(fs_code);
      prog.link();

      prog.set_attribute_location(shader_program::attribute_kind::vertex, 0);
      prog.set_attribute_location(shader_program::attribute_kind::color, 1);
    }

    {
      std::string vs_code, gs_code, fs_code;
      io::load_src("shaders\\wireframe.vert", vs_code);
      io::load_src("shaders\\wireframe.geom", gs_code);
      io::load_src("shaders\\wireframe.frag", fs_code);

      shader_program& prog = m_shader_manager.add("wireframe");
      prog.add_vertex_shader(vs_code);
      prog.add_geometry_shader(gs_code);
      prog.add_fragment_shader(fs_code);
      prog.link();

      prog.set_attribute_location(shader_program::attribute_kind::vertex, 0);
      prog.set_attribute_location(shader_program::attribute_kind::uv, 1);
      prog.set_need_height_field(true);  // TODO: pass here...
    }

    {
      std::string vs_code, gs_code, fs_code;
      io::load_src("shaders\\per_pixel_diffuse.vert", vs_code);
      io::load_src("shaders\\per_pixel_diffuse.geom", gs_code);
      io::load_src("shaders\\per_pixel_diffuse.frag", fs_code);

      shader_program& prog = m_shader_manager.add("per_pixel_diffuse");
      prog.add_vertex_shader(vs_code);
      prog.add_geometry_shader(gs_code);
      prog.add_fragment_shader(fs_code);
      prog.link();

      prog.set_attribute_location(shader_program::attribute_kind::vertex, 0);
      prog.set_attribute_location(shader_program::attribute_kind::uv, 1);
      prog.set_need_height_field(true);  // TODO: pass here...
      prog.set_need_normal_matrix(true);
      prog.set_need_model_view_matrix(true);
      prog.set_need_light_position(true);
    }

    {
      std::string vs_code, gs_code, fs_code;
      io::load_src("shaders\\normal_visualize.vert", vs_code);
      io::load_src("shaders\\normal_visualize.geom", gs_code);
      io::load_src("shaders\\normal_visualize.frag", fs_code);

      shader_program& prog = m_shader_manager.add("normal_visualize");
      prog.add_vertex_shader(vs_code);
      prog.add_geometry_shader(gs_code);
      prog.add_fragment_shader(fs_code);
      prog.link();

      prog.set_attribute_location(shader_program::attribute_kind::vertex, 0);
      prog.set_attribute_location(shader_program::attribute_kind::uv, 1);
      prog.set_need_normal_matrix(true);
      prog.set_need_height_field(true);  // TODO: pass here...
    }
  }

  // height field texture
  {
    glGenTextures(1, &m_height_map_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_height_map_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_height_field->width(), m_height_field->height(), 0, GL_RED, GL_FLOAT, m_height_field->data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // create axis mesh
  {
    float len = 1.0f;
    std::vector<vec3> vertices
    {
      vec3(0.0f, 0.0f, 0.0f),
      vec3(len, 0.0f, 0.0f),
      vec3(0.0f, 0.0f, 0.0f),
      vec3(0.0f, len, 0.0f),
      vec3(0.0f, 0.0f, 0.0f),
      vec3(0.0f, 0.0f, len)
    };

    std::vector<unsigned> indices
    {
      0, 1, 2, 3, 4, 5
    };

    std::vector<vec3> colors
    {
      vec3(1.0f, 0.0f, 0.0f),
      vec3(1.0f, 0.0f, 0.0f),
      vec3(0.0f, 1.0f, 0.0f),
      vec3(0.0f, 1.0f, 0.0f),
      vec3(0.0f, 0.0f, 1.0f),
      vec3(0.0f, 0.0f, 1.0f)
    };

    m_axis.reset(new mesh(vertices, indices, GL_LINES));
    m_axis->add_colors(colors);
  }

  // create terrain mesh
  {
    // create grid
    // 2x2 pixel -> 4 vertex
    //                       ^
    //                       |
    //                       z
    //     -------------------
    //     |3       |2       |
    //     |        |        |
    //     |        |        |
    //     ---------*--------*
    //     |1       |0       |
    //     |        |        |
    //     |        |        |
    // <-x----------*--------*
    const unsigned j_max(m_height_field->height());
    const unsigned i_max(m_height_field->width());

    std::vector<vec3> vertices;
    vertices.reserve(i_max * j_max);
    std::vector<vec2> uvs;
    uvs.reserve(i_max * j_max);

    math::range_mapper<unsigned, float> rmi(0, i_max - 1, 0.0f, 1.0f);
    math::range_mapper<unsigned, float> rmj(0, j_max - 1, 0.0f, 1.0f);
    for (auto j : math::range(0, j_max))
    {
      for (auto i : math::range(0, i_max))
      {
        vertices.push_back(vec3(i * m_height_field->resolution().s, 0.0, j * m_height_field->resolution().t));
        uvs.push_back(vec2(rmi.map(i), rmj.map(j)));
      }
    }

    // 2x2 pixel -> 2 triangle
    // 0, 2, 1
    // 1, 2, 3
    //                     ^
    //                     |
    //                     z
    //     -----------------
    //     |3      |2      |
    //     |       |       |
    //     |      3|      2|
    //     --------*-------*
    //     |1      |0     /|
    //     |       |   /   |
    //     |      1|/     0|
    // <-x---------*-------*
    std::vector<unsigned> indices;
    for (auto j = 0u, j_next = 1u; j_next < j_max; ++j, ++j_next)
    {
      for (auto i = 0u, i_next = 1u; i_next < i_max; ++i, ++i_next)
      {
        indices.push_back(i + i_max * j);
        indices.push_back(i + i_max * j_next);
        indices.push_back(i_next + i_max * j);

        indices.push_back(i_next + i_max * j);
        indices.push_back(i + i_max * j_next);
        indices.push_back(i_next + i_max * j_next);
      }
    }

    {
      m_meshes.push_back(mesh::ptr(new mesh(vertices, indices, GL_TRIANGLES)));
      mesh::ptr mesh = m_meshes.back();
      mesh->add_uvs(uvs);
      mesh->set_texture(m_height_map_texture_id);
      mesh->set_transformation(glm::rotate(mat4(1.0f), glm::radians(0.0f), vec3(1.0f, 0.0f, 0.0f)));
    }
  }
}

void GLApplication::render()
{
  glClearColor(m_background_color.x, m_background_color.y, m_background_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  vec3 light_position(100, 200, -4000);

  for (const auto& mesh : m_meshes)
  {
    if (m_draw_mode == draw_mode::shaded || m_draw_mode == draw_mode::shaded_wireframe)
    {
      mesh->render(m_shader_manager.get("per_pixel_diffuse"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
    }
    if (m_draw_mode == draw_mode::wireframe || m_draw_mode == draw_mode::shaded_wireframe)
    {
      mesh->render(m_shader_manager.get("wireframe"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
    }

    if (m_render_normals)
    {
      mesh->render(m_shader_manager.get("normal_visualize"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
    }
  }

  if (m_render_axis)
  {
    m_axis->render(m_shader_manager.get("simple_color"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
  }
}

void GLApplication::request_update()
{
  glutPostRedisplay();
}

void GLApplication::destroy_scene() noexcept
{
  m_height_field.reset();
  m_axis.reset();
  m_meshes.clear();
  m_shader_manager.clear();
  glDeleteTextures(1, &m_height_map_texture_id);
}

// static
const vec3& GLApplication::world_up()
{
  static const vec3 world_up(0.0f, 1.0f, 0.0f);
  return world_up;
}

// callbacks
void GLApplication::keyboard_callback(unsigned char character, int /*x*/, int /*y*/)
{
  GLApplication& app(instance());

  bool need_redraw(false);
  switch (character)
  {
    case 'w':
    {
      float dz = 1.0;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (-dz * forward) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 's':
    {
      float dz = -1.0;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (-dz * forward) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 'a':
    {
      float dx = -1.0;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (dx * strafe) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 'd':
    {
      float dx = 1.0;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (dx * strafe) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 'x':
    {
      app.m_render_axis = !app.m_render_axis;
      need_redraw = true;
      break;
    }
    case 'v':
    {
      break;
    }
    case 'n':
    {
      app.m_render_normals = !app.m_render_normals;
      need_redraw = true;
      break;
    }
    case 'm':
    {
      app.m_draw_mode = static_cast<draw_mode::Enum>(app.m_draw_mode + 1);
      if (app.m_draw_mode == draw_mode::count)
      {
        app.m_draw_mode = static_cast<draw_mode::Enum>(0);
      }
      need_redraw = true;
      break;
    }
    case 27:
    {
      app.destroy_scene();
      exit(0);
      break;
    }
    case 32:
    {
      need_redraw = true;
      break;
    }
  }

  if (need_redraw)
  {
    glutPostRedisplay();
  }
}

void GLApplication::display_callback()
{
  GLApplication& app(instance());
  app.render();
  glutSwapBuffers();
}

void GLApplication::reshape_callback(int w, int h)
{
  GLApplication& app(instance());
  w = w > 1 ? w : 1;
  h = h > 1 ? h : 1;

  app.m_camera.set_window_size(uvec2(w, h));
  app.m_camera.update_projection();

  glViewport(0, 0, app.m_camera.window_size().x, app.m_camera.window_size().y);
}

void GLApplication::mouse_button_callback(int button, int state, int x, int y)
{
  GLApplication& app = instance();

  app.m_mouse_left_down = button == GLUT_LEFT_BUTTON && state == GLUT_DOWN;

  if (app.m_mouse_left_down)
  {
    app.m_mouse_position.x = static_cast<float>(x);
    app.m_mouse_position.y = static_cast<float>(y);
  }
}

void GLApplication::mouse_move_callback(int x, int y)
{
  GLApplication& app = instance();
  
  bool need_redraw(false);
  if (app.m_mouse_left_down)
  {
    const vec2 current_mouse(static_cast<float>(x), static_cast<float>(y));
    const vec2 mouse_delta(current_mouse - app.m_mouse_position);

    app.m_camera.set_orientation(app.m_camera.orientation() + vec3(0.0, app.m_mouse_sensitivity * mouse_delta.y, app.m_mouse_sensitivity * mouse_delta.x));
    app.m_mouse_position = current_mouse;
    app.m_camera.update_view();

    need_redraw = true;
  }

  if (need_redraw)
  {
    glutPostRedisplay();
  }
}
}
