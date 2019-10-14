#pragma once

#include <vector>

#include "types.h"

namespace terrain
{
  class height_field
  {
  public:
    height_field(const unsigned width, const unsigned height, const vec2& resolution, const float* data)
      : m_width(width)
      , m_height(height)
      , m_resolution(resolution)
    {
      const auto count = m_width * m_width;
      m_data.reserve(count);
      m_data.insert(m_data.begin(), data, data + count);
    }

    const unsigned& width() const
    {
      return m_width;
    }
    const unsigned& height() const
    {
      return m_height;
    }
    const vec2& resolution() const
    {
      return m_resolution;
    }
    const float* data() const
    {
      return &m_data[0];
    }
  private:
    const unsigned m_width;
    const unsigned m_height;
    const vec2 m_resolution;
    std::vector<float> m_data;
  };
}
