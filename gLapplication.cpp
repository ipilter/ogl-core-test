#pragma once


#include <glm\gtc\matrix_transform.hpp>
#include "types.h"
#include "io.h"

#include "glapplication.h"

// ideas from http://www.opengl-tutorial.org/download/

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
{}

GLApplication::~GLApplication()
{
}

void GLApplication::init(int argc, char* argv[], const uvec2& window_size, const uvec2& opengl_version)
{
  m_window_size = window_size;

  if (argc < 1)
  {
    throw std::runtime_error("missing parameter");
  }

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

void GLApplication::render()
{
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_render_axis)
  {
    m_meshes[0]->render(m_shader_program, m_projection, m_view);
  }

  m_meshes[1]->render(m_shader_program, m_projection, m_view);
}

void GLApplication::request_update()
{
  glutPostRedisplay();
}

void GLApplication::update_projection()
{
  m_projection = glm::perspectiveFov(glm::radians(75.0), static_cast<double>(m_window_size.x), static_cast<double>(m_window_size.y), 0.01, 1000.0);
}

void GLApplication::create_shader(const std::string& vs_filepath, const std::string& fs_filepath)
{
  std::string v_src;
  io::load_src(vs_filepath, v_src);

  std::string f_src;
  io::load_src(fs_filepath, f_src);

  const char* vstr[] = { v_src.c_str() };
  const char* fstr[] = { f_src.c_str() };

  m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(m_vertex_shader, 1, vstr, NULL);
  glCompileShader(m_vertex_shader);

  GLint status = GL_FALSE;
  int info_size(0);

  // Check Vertex Shader
  glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    glGetShaderiv(m_vertex_shader, GL_INFO_LOG_LENGTH, &info_size);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetShaderInfoLog(m_vertex_shader, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }

    throw std::runtime_error(std::string("cannot compile vertex shader: ") + msg);
  }

  glShaderSource(m_fragment_shader, 1, fstr, NULL);
  glCompileShader(m_fragment_shader);

  // Check Fragment Shader
  glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    glGetShaderiv(m_fragment_shader, GL_INFO_LOG_LENGTH, &info_size);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetShaderInfoLog(m_fragment_shader, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }

    throw std::runtime_error(std::string("cannot compile fragment shader: ") + msg);
  }

  m_shader_program = glCreateProgram();
  glAttachShader(m_shader_program, m_vertex_shader);
  glAttachShader(m_shader_program, m_fragment_shader);
  glLinkProgram(m_shader_program);

  // Check the program
  glGetProgramiv(m_shader_program, GL_LINK_STATUS, &status);

  if (status == GL_FALSE)
  {
    glGetShaderiv(m_shader_program, GL_INFO_LOG_LENGTH, &info_size);
    std::string msg;
    if (info_size > 0)
    {
      std::string buffer(info_size + 1, ' ');
      glGetShaderInfoLog(m_shader_program, info_size, NULL, &buffer[0]);
      msg.swap(buffer);
    }

    throw std::runtime_error(std::string("cannot link shader program: ") + msg);
  }
}

void GLApplication::create_scene()
{
  // camera
  {
    vec3 world_up = vec3(0, 1, 0);
    vec3 eye(0.0f, 2.0f, 3.0f);
    vec3 target(0.0f, 0.0f, 0.0f);
    vec3 up = world_up;
    m_view = glm::lookAt(eye, target, up);
  }

  // shaders
  {
    create_shader("shaders\\simple_color.vert", "shaders\\simple_color.frag");
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

    std::vector<vec3> colors
    {
      vec3(0.7f, 0.0f, 0.0f),
      vec3(1.0f, 0.0f, 0.0f),
      vec3(0.0f, 0.7f, 0.0f),
      vec3(0.0f, 1.0f, 0.0f),
      vec3(0.0f, 0.0f, 0.7f),
      vec3(0.0f, 0.0f, 1.0f)
    };

    std::vector<unsigned> indices
    {
      0, 1, 2, 3, 4, 5
    };

    m_meshes.push_back(mesh::ptr(new mesh()));
    m_meshes[0]->create(vertices, colors, indices);
    m_meshes[0]->set_primitive_type(GL_LINES);
  }

  // create model mesh
  {
    std::vector<vec3> vertices
    {
      vec3(0.0f, 0.0f, 0.0f),
      vec3(1.0f, 0.0f, 0.0f),
      vec3(0.0f, 0.0f, 1.0f)
    };

    std::vector<vec3> colors
    {
      vec3(1.0f, 0.0f, 0.0f),
      vec3(0.0f, 1.0f, 0.0f),
      vec3(0.0f, 0.0f, 1.0f)
    };

    std::vector<unsigned> indices
    {
      0, 1, 2
    };

    m_meshes.push_back(mesh::ptr(new mesh()));
    m_meshes[1]->create(vertices, colors, indices);
    m_meshes[1]->set_primitive_type(GL_TRIANGLES);
  }
}

void GLApplication::destroy_scene() noexcept
{
  m_meshes.clear();
}

void GLApplication::flip_axis()
{
  m_render_axis = !m_render_axis;
}

// static
void GLApplication::keyboard_callback(unsigned char character, int x, int y)
{
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
    instance().flip_axis();
    need_redraw = true;
    break;
  }
  case 'v':
  {
    break;
  }
  case 27:
  {
    // cleanup
    exit(0); // say good bye
    break;
  }
  case 32:
  {
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

void GLApplication::terminate()
{
  instance().destroy_scene();
}
