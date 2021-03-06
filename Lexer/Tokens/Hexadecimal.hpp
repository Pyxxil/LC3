#ifndef TOKEN_HEXADECIMAL_HPP
#define TOKEN_HEXADECIMAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Hexadecimal : public Immediate {
public:
  explicit Hexadecimal(std::string s, size_t t_line, size_t t_column,
                       const std::string &t_file, bool is_negative = false)
      : Immediate(std::move(s), t_line, t_column,t_file) {

    std::string immediate = token;
    if (immediate.length() > 2 && 'X' == std::toupper(immediate[1])) {
      immediate.erase(0, 2);
    } else if ('X' == std::toupper(immediate.front())) {
      immediate.erase(0, 1);
    }

    if (immediate.length() > 4) {
      too_big = true;
      return;
    }

    char *check = nullptr;
    const auto v = std::strtoll(immediate.c_str(), &check, 16);
    if (nullptr == check || v > (2 * std::numeric_limits<int16_t>::max()) + 1) {
      too_big = true;
    } else {
      if (is_negative) {
        token = "-" + token;
      }
      m_value = static_cast<int16_t>(v);
    }
  }

  Hexadecimal(const Hexadecimal &) = default;
  Hexadecimal(Hexadecimal &&) = default;

  Hexadecimal &operator=(const Hexadecimal &) = default;
  Hexadecimal &operator=(Hexadecimal &&) = default;
};
} // namespace Token
} // namespace Lexer

#endif
