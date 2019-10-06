#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>

namespace io
{
  template<class T>
  T parseNumber(const std::string& str)
  {
    T t(0);
    if (!(std::istringstream(str) >> t))
    {
      throw std::runtime_error(std::string("cannot parse ") + str + " as number");
    }
    return t;
  }

  inline void load_src(const std::string& path, std::string& src)
  {
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
      std::stringstream ss;
      ss << "Could not open shader source file [";
      ss << path;
      ss << "]";
      throw std::runtime_error(ss.str());
    }

    std::stringstream ss;
    ss << file.rdbuf();

    src.assign(ss.str());
  }
}
