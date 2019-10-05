#pragma once

#include "types.h"

namespace math
{
  inline vec3 project(const vec3& a, const vec3& b)
  {
    return glm::dot(a, b) * b;
  }
}
