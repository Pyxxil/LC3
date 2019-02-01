#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <iterator>

namespace Algorithm {

template <class InputIterator, class Function>
constexpr bool any(InputIterator first, InputIterator last, Function f) {
  for (; first != last; ++first) {
    if (f(*first)) {
      return true;
    }
  }

  return false;
}

template <class InputIterator, class Function>
constexpr bool all(InputIterator first, InputIterator last, Function f) {
  for (; first != last; ++first) {
    if (!f(*first)) {
      return false;
    }
  }

  return true;
}

template <typename T> struct greater_than {
  constexpr greater_than(T next, T prev) : n(next), p(prev) {}
  constexpr bool operator()() { return n > p; }

  const T n, p;
};

template <typename T> struct less_than {
  constexpr less_than(T next, T prev) : n(next), p(prev) {}
  constexpr bool operator()() { return n < p; }

  const T n, p;
};

template <class InputIterator,
          class Comp = greater_than<
              typename std::iterator_traits<InputIterator>::value_type>>
constexpr auto max(InputIterator first, InputIterator last) {
  auto _max{*first};
  for (; ++first != last;) {
    if (Comp(*first, _max)()) {
      _max = *first;
    }
  }

  return _max;
}

template <class InputIterator, class Comp>
constexpr auto max(InputIterator first, InputIterator last, Comp compare) {
  auto _max{*first};
  for (; ++first != last;) {
    if (compare(*first, _max)) {
      _max = *first;
    }
  }

  return _max;
}

template <class InputIterator,
          class Comp = less_than<
              typename std::iterator_traits<InputIterator>::value_type>>
constexpr auto min(InputIterator first, InputIterator last) {
  auto _min{*first};
  for (; ++first != last;) {
    if (Comp(*first, _min)()) {
      _min = *first;
    }
  }

  return _min;
}

template <class InputIterator, class Comp>
constexpr auto min(InputIterator first, InputIterator last, Comp compare) {
  auto _min{*first};
  for (; ++first != last;) {
    if (compare(*first, _min)) {
      _min = *first;
    }
  }

  return _min;
}

template <class InputIterator, class T>
constexpr T accumulate(InputIterator first, InputIterator last, T init) {
  T sum = init;
  for (; first != last; ++first) {
    sum += *first;
  }

  return sum;
}

template <class InputIterator, class T>
constexpr T accumulate2d(InputIterator first, InputIterator last, T init) {
  T sum = init;
  for (; first != last; ++first) {
    for (auto iter = first.begin(); iter != first.end(); ++iter) {
      sum += *iter;
    }
  }
  return sum;
}

template <class InputIterator, class T, class Function>
constexpr T accumulate2d(InputIterator first, InputIterator last, T init,
                         Function f) {
  T sum = init;
  for (; first != last; ++first) {
    for (auto iter = first->begin(); iter != first->end(); ++iter) {
      sum = f(sum, *iter);
    }
  }
  return sum;
}

template <class InputIterator, class Function>
constexpr Function enumerate(InputIterator first, InputIterator last,
                             Function f, long long start = 0) {
  for (; first != last; ++first, ++start) {
    f(*first, start);
  }

  return f;
}

template <class InputIterator, class Function, class _Function>
constexpr _Function first(InputIterator first, InputIterator last, Function s,
                          _Function f, long long start = 0) {
  s(*first, start++);

  for (; ++first != last; ++start) {
    f(*first, start);
  }

  return f;
}

template <class InputIterator, class Function, class _Function>
constexpr _Function nth(size_t idx, InputIterator first, InputIterator last,
                        Function s, _Function f) {
  for (size_t pos = 0; first != last; ++first, ++pos) {
    if (idx == pos) {
      s(*first, pos);
    }
    f(*first, pos);
  }

  return f;
}

template <class InputIterator, class Function, class _Function>
constexpr _Function after(size_t idx, InputIterator first, InputIterator last,
                          Function s, _Function f) {
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
} // namespace Algorithm

#endif
