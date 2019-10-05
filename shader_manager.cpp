#include <stdexcept>
#include <sstream>

#include "opengl.h"
#include "shader_manager.h"
#include "io.h"

namespace opengl
{
  shader_manager::shader_manager()
  { }

  shader_manager::~shader_manager()
  {
    remove_all();
  }

  void shader_manager::bind (const std::string& name)
  {
    m_shaderPrograms[name]->bind();
  }

  void shader_manager::unbind ()
  {
    glUseProgram(0);
  }

  void shader_manager::create (const std::string& name, const std::string& vsFileName, const std::string& fsFileName)
  {
    if (m_shaderPrograms.find(name) != m_shaderPrograms.end()) {
      std::stringstream ss;
      ss << "Shader program with name [";
      ss << name;
      ss << "] already exist!";
      throw std::runtime_error(ss.str());
    }

    std::string vs, fs;
    io::load_src(vsFileName, vs);
    io::load_src(fsFileName, fs);
    shader_program* theShaderProgram = new shader_program(vs, fs);

    if (theShaderProgram == nullptr)
    {
      throw std::runtime_error("out of memory");
    }

    if (!theShaderProgram->IsValid())
    {
      std::string vertexLog;
      std::string fragmentLog;
      theShaderProgram->GetVertexInfoLog(vertexLog);
      theShaderProgram->GetFragmentInfoLog(fragmentLog);
      
      std::stringstream ss;
      ss << "Could not create shader program!\n vertex log is:\n";
      ss << vertexLog;
      ss << "\n\n fragment log is:\n";
      ss << fragmentLog;
      throw std::runtime_error (ss.str ());
    }

    m_shaderPrograms[name] = theShaderProgram;
  }

  void shader_manager::remove_all (void)
  {
    for (std::map<std::string, shader_program*>::iterator it = m_shaderPrograms.begin() ; it != m_shaderPrograms.end(); ++it )
    {
      delete it->second;
    }
    m_shaderPrograms.clear ();
  }

  void shader_manager::remove (const std::string& name)
  {
    if (m_shaderPrograms.find (name) != m_shaderPrograms.end ())
    {
      shader_program* deletable = m_shaderPrograms[name];
      delete deletable;
      m_shaderPrograms.erase (name);
    }
  }

  shader_program* shader_manager::get_program (const std::string& name)
  {
    if (m_shaderPrograms.find (name) == m_shaderPrograms.end ()) {
      std::stringstream ss;
      ss << "Shader program with name [";
      ss << name;
      ss << "] does not exist!";
      throw std::runtime_error (ss.str ());
    }
    return m_shaderPrograms[name];
  }
};