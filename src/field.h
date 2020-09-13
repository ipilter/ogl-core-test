#pragma once

#include <memory>
#include <vector>

namespace terrain
{
// field specialized for discrete 2d values
// maps x,y to T
// T is nullable
  template<typename T>
  class field
  {
  public:
    using value_t = T;
    using ptr = std::shared_ptr<field>;
    using buffer_t = std::vector<value_t>;

  public:
    class perpixel_generator
    {
    public:
      perpixel_generator(field<value_t>& f)
        : m_field(f)
      { }
    public:
      virtual value_t operator ()(const uvec2& pos) const = 0;

    public:
      void generate()
      {
        uvec2 pos;
        for (pos.y = 0; pos.y < m_field.size().y; ++pos.y)
          for (pos.x = 0; pos.x < m_field.size().x; ++pos.x)
            m_field(pos) = (*this)(pos);
      }

    protected:
      field<value_t>& m_field;
    };

  public:
    field(const uvec2& size)
      : m_size(size)
      , m_buffer(m_size.s * m_size.t, static_cast<value_t>(0))
    { }

    const value_t& operator()(const uvec2& pos) const
    {
      return m_buffer[index(pos)];
    }

    value_t& operator()(const uvec2& pos)
    {
      return m_buffer[index(pos)];
    }

    const uvec2& size() const
    {
      return m_size;
    }

    const value_t* data() const
    {
      return &m_buffer[0];
    }

    void swap_data(buffer_t& data)
    {
      m_buffer.swap(data);
    }

  private:
    unsigned index(const uvec2& pos) const
    {
      return pos.s + m_size.s * pos.t;
    }

  private:
    uvec2 m_size;
    buffer_t m_buffer;
  };
}
