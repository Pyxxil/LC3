#ifndef TOKEN_BINARY_HPP
#define TOKEN_BINARY_HPP

#include <bitset>

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Binary : public Immediate {
public:
  explicit Binary(std::string s, size_t t_line, size_t t_column,
                  const std::string &t_file, bool is_negative = false)
      : Immediate(std::move(s), t_line, t_column, t_file) {
    std::string immediate = token;
    if (immediate.length() > 2 && 'B' == std::toupper(immediate[1])) {
      immediate.erase(0, 2);
    } else if ('B' == std::toupper(immediate.front())) {
      immediate.erase(0, 1);
    }

    if (immediate.length() > 16) {
      too_big = true;
    } else {
      m_value =
          is_negative
              ? -static_cast<int16_t>(std::bitset<16>(immediate).to_ulong())
              : static_cast<int16_t>(std::bitset<16>(immediate).to_ulong());
    }
  }

  Binary(const Binary &) = default;
  Binary(Binary &&) = default;

  Binary &operator=(const Binary &) = default;
  Binary &operator=(Binary &&) = default;
};
} // namespace Token
} // namespace Lexer

#endif
