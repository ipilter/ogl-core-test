#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>

namespace io
{
template<class T>
T parse_number(const std::string& str)
{
  T t(0);
  if (!(std::istringstream(str) >> t))
  {
    throw std::runtime_error(std::string( "cannot parse " ) + str + " as number");
  }
  return t;
}

inline void read_text_file(const std::string& path, std::string& src)
{
  std::ifstream file( path.c_str());
  if (!file.is_open())
  {
    throw std::runtime_error( std::string( "Could not open file: " ) + path );
  }

  std::stringstream ss;
  ss << file.rdbuf();

  src.assign(ss.str());
}
}
