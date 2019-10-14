#pragma once

namespace math
{
template<class T>
class range_t
{
public:
  class iterator
  {
    friend class range_t;
  public:
    iterator& operator++()
    {
      ++m_position;
      return *this;
    }

    iterator operator++(T)
    {
      iterator copy(*this);
      ++m_position;
      return copy;
    }

    bool operator == (const iterator& rhs) const
    {
      return m_position == rhs.m_position;
    }

    bool operator != (const iterator& rhs) const
    {
      return m_position != rhs.m_position;
    }

    T operator* () const
    {
      return m_position;
    }

  protected:
    iterator(const T start)
      : m_position(start)
    {}

  private:
    T m_position;
  };

public:
  range_t(const T begin, const T end)
    : m_begin(begin)
    , m_end(end)
  {}

  const iterator& begin() const
  {
    return m_begin;
  }

  const iterator& end() const
  {
    return m_end;
  }

  const iterator m_begin;
  const iterator m_end;
};

using range = range_t<int>;
}
