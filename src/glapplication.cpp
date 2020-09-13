#pragma once

#include <fstream>
#include <iomanip>

#include "types.h"
#include "io.h"
#include "glapplication.h"
#include "range.h"
#include "range_mapper.h"
#include "ppm.h"

namespace opengl
{
GLApplication& GLApplication::instance()
{
  static GLApplication app;
  return app;
}

GLApplication::GLApplication()
  : m_background_color(0)
  , m_mouse_left_down(false)
  , m_mouse_position(0.0)
  , m_mouse_sensitivity(0.3f)
  , m_keyboard_speed(0.3f)
  , m_keyboard_step(0.5f)
  , m_settings(vec3(-1.0f, 2.0f, 3.0f), vec3(0.0f, 20.0f, 45.0f), draw_mode::shaded_wireframe, false, true)
{}

GLApplication::~GLApplication()
{}

void GLApplication::parse_settings(const std::string& path)
{
  std::ifstream stream(path);
  if (stream.is_open())
  {
    stream >> m_settings.camera_position.x;
    stream >> m_settings.camera_position.y;
    stream >> m_settings.camera_position.z;

    stream >> m_settings.camera_orientation.x;
    stream >> m_settings.camera_orientation.y;
    stream >> m_settings.camera_orientation.z;

    int temp(0);
    stream >> temp;
    m_settings.m_draw_mode = static_cast<draw_mode::Enum>(temp);
    stream >> m_settings.m_render_axis;
    stream >> m_settings.m_render_normals;
  }
}

void GLApplication::save_settings(const std::string& path)
{
  m_settings.camera_position = m_camera.position();
  m_settings.camera_orientation = m_camera.orientation();

  std::ofstream stream(path);
  if (stream.is_open())
  {
    stream << std::fixed << std::setprecision(6) << m_settings.camera_position.x << " " << m_settings.camera_position.y << " " << m_settings.camera_position.z << "\n";
    stream << m_settings.camera_orientation.x << " " << m_settings.camera_orientation.y << " " << m_settings.camera_orientation.z << "\n";
    stream << m_settings.m_draw_mode << "\n";
    stream << m_settings.m_render_axis << "\n";
    stream << m_settings.m_render_normals << "\n";
  }
}

void GLApplication::init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version)
{
  if (argc < 1)
  {
    throw std::runtime_error("missing parameter(s) valid parmaeters: [,]");
  }

  const std::string settings_file_path("settings.xml");
  parse_settings(settings_file_path);

  m_camera.set_window_size(window_size);
  m_camera.set_position(m_settings.camera_position);
  m_camera.set_orientation(m_settings.camera_orientation);

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
  const uvec2 size(3, 2);
  terrain::height_field::value_t h_min(100000), h_max(-100000);
  {
    terrain::height_field::buffer_t height_field_pixels(size.s * size.t, 0.0f);
    //vec2 resolution(1.0f, 1.0f);
    //vec2 a(2.0f);
    //vec2 b(50.0f);
    //for (auto j = 0u; j < size.t; ++j)
    //{
    //  for (auto i = 0u; i < size.s; ++i)
    //  {
    //    const terrain::height_field::value_t value = a.x * sin((static_cast<terrain::height_field::value_t>(j) / size.t) * b.x) + a.y * cos((static_cast<terrain::height_field::value_t>(i) / size.s) * b.y);
    //    height_field_pixels[i + j * size.s] = value;
    //    if(value < h_min) h_min = value;
    //    if (value > h_max) h_max = value;
    //  }
    //}

    //vec2 resolution(0.1f, 0.1f);
    //float value = 0.1f;
    //height_field_pixels[2 + 2 * size.s] = value;
    //height_field_pixels[3 + 2 * size.s] = value / 1.0f;
    //height_field_pixels[3 + 3 * size.s] = value / 1.0f;
    //height_field_pixels[2 + 3 * size.s] = value / 1.0f;
    //h_min = 0.0f;
    //h_max = value;

    vec2 resolution(0.1f, 0.1f);
    height_field_pixels[0 + 0 * size.s] = 1.0f;
    h_min = 0.0f;
    h_max = 1.0f;

    m_height_field.reset(new terrain::height_field(size, vec2(1.1f, 1.1f)));
    m_height_field->swap_data(height_field_pixels);

    //{
    //  ppm img( m_height_field->size() );
    //  math::range_mapper<float, ppm::pixel::channel_t> rm( h_min, h_max, 0, 255 );
    //  for ( auto j = 0u; j < size.t; ++j )
    //  {
    //    for ( auto i = 0u; i < size.s; ++i )
    //    {
    //      const float h = ( *m_height_field )( uvec2( i, j ) );
    //      ppm::pixel::channel_t gray( rm.map( h ) );
    //      img.set_pixel( uvec2( i, j ), ppm::pixel( gray, gray, gray ) );
    //    }
    //  }
    //  img.write( "height_field.ppm" );
    //}
  }

  // height field texture
  {
    glGenTextures(1, &m_height_field_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_height_field_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_height_field->size().s, m_height_field->size().t, 0, GL_RED, GL_FLOAT, m_height_field->data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // shaders
  {
    {
      std::string vs_code, fs_code;
      io::read_text_file("shaders\\simple_color.vert", vs_code);
      io::read_text_file("shaders\\simple_color.frag", fs_code);

      shader_program& prog = m_shader_manager.add("simple_color");
      prog.add_vertex_shader(vs_code);
      prog.add_fragment_shader(fs_code);
      prog.link();

      prog.set_attribute_location(shader_program::attribute_kind::vertex, 0);
      prog.set_attribute_location(shader_program::attribute_kind::color, 1);
    }

    {
      std::string vs_code, gs_code, fs_code;
      io::read_text_file("shaders\\wireframe.vert", vs_code);
      io::read_text_file("shaders\\wireframe.geom", gs_code);
      io::read_text_file("shaders\\wireframe.frag", fs_code);

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
      io::read_text_file("shaders\\per_pixel_diffuse.vert", vs_code);
      io::read_text_file("shaders\\per_pixel_diffuse.geom", gs_code);
      io::read_text_file("shaders\\per_pixel_diffuse.frag", fs_code);

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
      io::read_text_file("shaders\\normal_visualize.vert", vs_code);
      io::read_text_file("shaders\\normal_visualize.geom", gs_code);
      io::read_text_file("shaders\\normal_visualize.frag", fs_code);

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

    float intesity = 0.8f;
    std::vector<vec3> colors
    {
      vec3(intesity, 0.0f, 0.0f),
      vec3(intesity, 0.0f, 0.0f),
      vec3(0.0f, intesity, 0.0f),
      vec3(0.0f, intesity, 0.0f),
      vec3(0.0f, 0.0f, intesity),
      vec3(0.0f, 0.0f, intesity)
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
    const unsigned i_max(m_height_field->size().s);
    const unsigned j_max(m_height_field->size().t);

    std::vector<vec3> vertices;
    vertices.reserve(i_max * j_max);
    std::vector<vec2> uvs;
    uvs.reserve(i_max * j_max);

    math::range_mapper<size_t, terrain::height_field::value_t> rmi(0u, i_max - 1u, 0.0f, 1.0f);
    math::range_mapper<size_t, terrain::height_field::value_t> rmj(0u, j_max - 1u, 0.0f, 1.0f);
    for ( auto j : math::range(0, j_max))
    {
      for ( auto i : math::range(0, i_max))
      {
        vertices.push_back(vec3(i * m_height_field->resolution().s, j * m_height_field->resolution().t, 0.0));
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
      mesh->set_height_field_texture(m_height_field_texture_id);
      mesh->set_transformation(glm::rotate(glm::scale(mat4(1.0f), vec3(1.0f, -1.0f, 1.0f)), glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f)));
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
    if (m_settings.m_draw_mode == draw_mode::shaded || m_settings.m_draw_mode == draw_mode::shaded_wireframe)
    {
      mesh->render(m_shader_manager.get("per_pixel_diffuse"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
    }
    if (m_settings.m_draw_mode == draw_mode::wireframe || m_settings.m_draw_mode == draw_mode::shaded_wireframe)
    {
      mesh->render(m_shader_manager.get("wireframe"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
    }

    if (m_settings.m_render_normals)
    {
      mesh->render(m_shader_manager.get("normal_visualize"), m_camera.view_matrix(), m_camera.projection_matrix(), light_position);
    }
  }

  if (m_settings.m_render_axis)
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
  glDeleteTextures(1, &m_height_field_texture_id);
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
      float dz = app.m_keyboard_step;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (-dz * forward) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 's':
    {
      float dz = -app.m_keyboard_step;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (-dz * forward) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 'a':
    {
      float dx = -app.m_keyboard_step;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (dx * strafe) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 'd':
    {
      float dx = app.m_keyboard_step;
      glm::mat4 mat = app.m_camera.view_matrix();

      glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

      app.m_camera.set_position(app.m_camera.position() + (dx * strafe) * app.m_keyboard_speed);
      need_redraw = true;
      break;
    }
    case 'x':
    {
      app.m_settings.m_render_axis = !app.m_settings.m_render_axis;
      need_redraw = true;
      break;
    }
    case 'v':
    {
      break;
    }
    case 'n':
    {
      app.m_settings.m_render_normals = !app.m_settings.m_render_normals;
      need_redraw = true;
      break;
    }
    case 'm':
    {
      app.m_settings.m_draw_mode = static_cast<draw_mode::Enum>(app.m_settings.m_draw_mode + 1);
      if (app.m_settings.m_draw_mode == draw_mode::count)
      {
        app.m_settings.m_draw_mode = static_cast<draw_mode::Enum>(0);
      }
      need_redraw = true;
      break;
    }
    case 'C':
    {
      const std::string settings_file_path("settings.xml");
      app.save_settings(settings_file_path);
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
