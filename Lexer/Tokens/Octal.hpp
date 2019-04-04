#ifndef TOKEN_OCTAL_HPP
#define TOKEN_OCTAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Octal : public Immediate {
public:
  explicit Octal(std::string s, size_t t_line, size_t t_column,
                 const std::string &t_file, bool is_negative = false)
      : Immediate(std::move(s), t_line, t_column, t_file) {
    char *check = nullptr;
    const auto v = std::strtoull(token.c_str(), &check, 8);

    if (nullptr == check || v > (2 * std::numeric_limits<int16_t>::max() + 1)) {
      too_big = true;
    } else {
      if (is_negative) {
        token = "-" + token;
      }
      m_value = static_cast<std::int16_t>(v);
    }
  }

  Octal(const Octal &) = default;
  Octal(Octal &&) = default;

  Octal &operator=(const Octal &) = default;
  Octal &operator=(Octal &&) = default;
};
#endif
} // namespace Token
} // namespace Lexer

#endif
