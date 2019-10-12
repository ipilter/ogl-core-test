#pragma once
// link must have libraries

#include <stdexcept>

#include <gl\gl3w.h>

#pragma warning( push )
#pragma warning( disable : 4505 )
#include <gl\freeglut.h>
#pragma warning( pop )

namespace opengl
{
inline void check_gl(void)
{
#ifdef _DEBUG
  const GLenum gl_error(glGetError());
  if (!gl_error)
  {
    return;
  }

  switch (gl_error)
  {
    case 0x0500:
    {
      throw std::runtime_error("GL_INVALID_ENUM An unacceptable value is specified for an enumerated argument. GLenum argument out of range.");
      break;
    }
    case 0x0501:
    {
      throw std::runtime_error("GL_INVALID_VALUE A numeric argument is out of range. Generated if range is negative.");
      break;
    }
    case 0x0502:
    {
      throw std::runtime_error("GL_INVALID_OPERATION The specified operation is not allowed in the current state.");
      break;
    }
    case 0x0503:
    {
      throw std::runtime_error("GL_STACK_OVERFLOW This command would cause a stack overflow.");
      break;
    }
    case 0x0504:
    {
      throw std::runtime_error("GL_STACK_UNDERFLOW This command would cause a stack underflow.");
      break;
    }
    case 0x0505:
    {
      throw std::runtime_error("GL_OUT_OF_MEMORY There is not enough memory left to execute the command. The state of the GL is undefined.");
      break;
    }
    default:
    {
      throw std::runtime_error("Unrecognised error.");
      break;
    }
  }
#endif
}
};
