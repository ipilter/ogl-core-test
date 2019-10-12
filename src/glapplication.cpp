#pragma once

#include <ppm.h>

#include "types.h"
#include "io.h"
#include "glapplication.h"


namespace opengl
{
GLApplication& GLApplication::instance()
{
  static GLApplication app;
  return app;
}

GLApplication::GLApplication()
  : m_window_size(0)
  , m_projection(1)
  , m_view(1)
  , m_render_axis(true)
  , m_render_normals(false)
{}

GLApplication::~GLApplication()
{}

void GLApplication::init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version)
{
  if (argc < 1)
  {
    throw std::runtime_error("missing parameter");
  }

  m_window_size = window_size;

  glutInit(&argc, argv);
  glutInitContextVersion(opengl_version.x, opengl_version.y);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(m_window_size.x, m_window_size.y);
  glutCreateWindow("gl");

  if (gl3wInit())
  {
    throw std::runtime_error("failed to initialize OpenGL");
  }

  create_scene();

  // glut callbacks
  glutReshapeFunc(reshape_callback);
  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keyboard_callback);
}

void GLApplication::run()
{
  glutMainLoop();
}

void GLApplication::create_scene()
{
  // camera
  {
    vec3 eye(3.5f, 6.0f, -2.0f);
    vec3 target(3.5f, 0.0f, 2.0f);
    vec3 up = world_up();

    m_view = glm::lookAt(eye, target, up);
  }

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
      std::string vs_code, fs_code;
      io::load_src("shaders\\per_pixel_diffuse.vert", vs_code);
      io::load_src("shaders\\per_pixel_diffuse.frag", fs_code);

      shader_program& prog = m_shader_manager.add("per_pixel_diffuse");
      prog.add_vertex_shader(vs_code);
      prog.add_fragment_shader(fs_code);
      prog.link();

      prog.set_attribute_location(shader_program::attribute_kind::vertex, 0);
      prog.set_attribute_location(shader_program::attribute_kind::uv, 1);
      prog.set_need_texture(true);  // TODO: pass here...
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
      //prog.set_need_texture(true);  // TODO: pass here...
    }
  }

  // texture
  {
    glGenTextures(1, &m_height_map_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_height_map_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const int width = 2, height = 2;
    float pixels[] = { 0.2f, 0.4f,
                       0.6f, 0.8f };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RED, GL_FLOAT, pixels);
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

    m_meshes.push_back(mesh::ptr(new mesh(vertices, indices, GL_LINES)));
    mesh::ptr mesh = m_meshes.back();
    mesh->add_colors(colors);
  }

  // create model mesh
  {
    std::vector<vec3> vertices { vec3(0.0f, 0.0f, 0.0f),
                                 vec3(0.0f, 0.0f, 1.0f),
                                 vec3(1.0f, 0.0f, 0.0f),
                                 vec3(1.0f, 0.0f, 1.0f) };

    std::vector<vec2> uvs { vec2(0.0f, 0.0f),
                            vec2(1.0f, 0.0f),
                            vec2(0.0f, 1.0f),
                            vec2(1.0f, 1.0f) };

    std::vector<unsigned> indices { 0, 1, 2,
                                    2, 1, 3 };

    {
      m_meshes.push_back(mesh::ptr(new mesh(vertices, indices, GL_TRIANGLES)));
      mesh::ptr mesh = m_meshes.back();
      
      mesh->add_uvs(uvs);
      mesh->set_texture(m_height_map_texture_id);
      mesh->set_transform(glm::rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 1.0f, 0.0f)));
    }
  }
}

void GLApplication::render()
{
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_render_axis)
  {
    m_meshes[0]->render(m_shader_manager.get("simple_color"), m_projection * m_view);
  }

  for (size_t i = 1; i < m_meshes.size(); ++i)
  {
    if (m_render_normals)
    {
      m_meshes[i]->render(m_shader_manager.get("normal_visualize"), m_projection * m_view);
    }
    m_meshes[i]->render(m_shader_manager.get("per_pixel_diffuse"), m_projection * m_view);
  }
}

void GLApplication::request_update()
{
  glutPostRedisplay();
}

void GLApplication::update_projection()
{
  m_projection = glm::perspectiveFov(glm::radians(75.0), static_cast<double>(m_window_size.x), static_cast<double>(m_window_size.y), 0.01, 1000.0);
}

void GLApplication::destroy_scene() noexcept
{
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
  GLApplication& app = instance();
  
  bool need_redraw(false);
  switch (character)
  {
    case 'r':
    {
      break;
    }
    case 'u':
    {
      break;
    }
    case 's':
    {
      break;
    }
    case 'a':
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
  instance().render();
  glutSwapBuffers();
}

void GLApplication::reshape_callback(int w, int h)
{
  instance().m_window_size.x = w > 1 ? w : 1;
  instance().m_window_size.y = h > 1 ? h : 1;

  glViewport(0, 0, instance().m_window_size.x, instance().m_window_size.y);
  instance().update_projection();
}
}
