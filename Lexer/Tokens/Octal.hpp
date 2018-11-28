#ifndef TOKEN_OCTAL_HPP
#define TOKEN_OCTAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Octal : public Immediate {
public:
  explicit Octal(std::string s, size_t tLine, size_t tColumn,
                 const std::string &tFile, bool isNegative = false)
      : Immediate(std::move(s), tLine, tColumn, tFile) {
    char *check = nullptr;
    const auto v = std::strtoull(token.c_str(), &check, 8);

    if (nullptr == check || v > (2 * std::numeric_limits<int16_t>::max() + 1)) {
      tooBig = true;
    } else {
      if (isNegative) {
        token = "-" + token;
      }
      mValue = static_cast<std::int16_t>(v);
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
