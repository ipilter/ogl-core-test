#pragma once

#include "types.h"
#include "field.h"

namespace terrain
{
  class height_field : public field<float>
  {
  public:
    using ptr = std::shared_ptr<height_field>;

    height_field(const uvec2& size, const vec2& resolution)
      : field<float>(size)
      , m_resolution(resolution)
    { }

    const vec2& resolution() const
    {
      return m_resolution;
    }

  private:
    const vec2 m_resolution;
  };
}
