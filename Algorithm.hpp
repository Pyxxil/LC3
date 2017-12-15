#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <functional>
#include <vector>

namespace Algorithm {
template<class InputIterator, class Function>
Function
enumerate(InputIterator first, InputIterator last, Function f)
{
  for (size_t pos = 0; first != last; ++first, ++pos) {
    f(*first, pos);
  }

  return f;
}

template<class InputIterator, class Function, class _Function>
_Function
first(InputIterator first, InputIterator last, Function s, _Function f)
{
  s(*first, 0);
  ++first;

  for (size_t pos = 1; first != last; ++first, ++pos) {
    f(*first, pos);
  }

  return f;
}

template<class InputIterator, class Function, class _Function>
_Function
nth(size_t idx,
    InputIterator first,
    InputIterator last,
    Function s,
    _Function f)
{
  for (size_t pos = 0; first != last; ++first, ++pos) {
    if (idx == pos) {
      s(*first, pos);
    }
    f(*first, pos);
  }

  return f;
}

template<class InputIterator, class Function, class _Function>
_Function
after(size_t idx,
      InputIterator first,
      InputIterator last,
      Function s,
      _Function f)
{
  size_t pos = 0;
  for (; pos <= idx && first != last; ++first, ++pos) {
    f(*first, pos);
  }

  for (; first != last; ++first, ++pos) {
    s(*first, pos);
    f(*first, pos);
  }

  return f;
}

template<class InputIterator, class Function>
class Enumerator
{
public:
  Enumerator(InputIterator f, InputIterator l)
    : m_first(f)
    , m_last(l)
  {}

  Enumerator(const Enumerator&) = default;
  Enumerator(Enumerator&&) noexcept = default;

  Enumerator& operator=(const Enumerator&) = default;
  Enumerator& operator=(Enumerator&&) noexcept = default;

  ~Enumerator() = default;

  Enumerator& nth(size_t idx, Function f)
  {
    (void)idx;
    (void)f;
    return *this;
  }

  Enumerator& first(Function f)
  {
    (void)f;
    return *this;
  }

  Enumerator& every(size_t idx, Function f)
  {
    (void)idx;
    (void)f;
    return *this;
  }

private:
  InputIterator m_first;
  InputIterator m_last;
};
}

#endif
