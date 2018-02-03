#ifndef TOKEN_HEXADECIMAL_HPP
#define TOKEN_HEXADECIMAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Hexadecimal : public Immediate {
public:
  explicit Hexadecimal(std::string s, size_t tLine, size_t tColumn,
                       const std::string &tFile, bool isNegative = false)
      : Immediate(std::move(s), tLine, tColumn, tFile) {

    std::string immediate = token;
    if (immediate.length() > 2 && 'X' == std::toupper(immediate[1])) {
      immediate.erase(0, 2);
    } else if ('X' == std::toupper(immediate.front())) {
      immediate.erase(0, 1);
    }

    if (immediate.length() > 4) {
      tooBig = true;
      return;
    }

    char *check = nullptr;
    const auto v = std::strtoll(immediate.c_str(), &check, 16);
    if (nullptr == check || v > (2 * std::numeric_limits<int16_t>::max()) + 1) {
      tooBig = true;
    } else {
      if (isNegative) {
        token = "-" + token;
      }
      mValue = static_cast<int16_t>(v);
    }
  }

  Hexadecimal(const Hexadecimal &) = default;
  Hexadecimal(Hexadecimal &&) noexcept = default;

  Hexadecimal &operator=(const Hexadecimal &) = default;
  Hexadecimal &operator=(Hexadecimal &&) noexcept = default;
};
} // namespace Token
} // namespace Lexer

#endif
