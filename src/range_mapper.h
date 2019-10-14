#pragma once

namespace math 
{
/**
 * Maps value 's' from range [a1,a2] into [b1,b2].
 *      b1+(s-a1)(b2-b1)
 * s'=  ----------------
 *          (a2-a1)
 *
 */

// Unsigned specialization
template<class F, class T>
class range_mapper
{
public:
  range_mapper(const F a1, const F a2, const T b1, const T b2)
    : m_a1(a1)
    , m_a2(a2)
    , m_b1(b1)
    , m_b2(b2)
  {}
public:
  T map(const F s) const
  {
    F a_norm( m_a2 - m_a1 );
    if (a_norm == 0)
    {
      return 0;
    }
    return T( ( m_b1 + ( s - static_cast<T>(m_a1) ) * ( m_b2 - m_b1 ) ) / static_cast<T>(a_norm));
  }
private:
  F m_a1;
  F m_a2;
  T m_b1;
  T m_b2;
};
} // namespace math
