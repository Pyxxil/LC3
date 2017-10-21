#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifndef NDEBUG
#define debug(x)                                                               \
  std::cerr << "DEBUG: " << __FILE__ << ": " << __LINE__ << ": "               \
            << __FUNCTION__ << ": " << x << '\n'
#else
#define debug(_)
#endif
#endif