#include <iostream>
#include <stdexcept>
#include <sstream>

#include "glapplication.h"

int main(int argc, char* argv[])
{
  int ret = 0;
  try
  {
    GLApplication& app = GLApplication::instance();
    app.init(argc, argv, uvec2(1920/4, 1080/4), uvec2(4, 5));
    atexit(app.terminate);  // static? crash?

    app.run();
  }
  catch (const std::exception& e)
  {
    std::cout << "Error: " << e.what() << std::endl;
  }
  return ret;
}
