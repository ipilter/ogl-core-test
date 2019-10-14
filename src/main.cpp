#include <iostream>
#include <stdexcept>
#include <sstream>

#include "glapplication.h"

int main(int argc, char* argv[])
{
  try
  {
    opengl::GLApplication& app(opengl::GLApplication::instance());
    app.init(argc, argv, uvec2(1920 / 2, 1080 / 2), uvec2(4, 5));
    app.run();
  }
  catch (const std::exception& e)
  {
    std::cout << "Error: " << e.what() << std::endl;
  }
  return 0;
}
