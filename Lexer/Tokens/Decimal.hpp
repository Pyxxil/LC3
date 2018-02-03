#ifndef TOKEN_DECIMAL_HPP
#define TOKEN_DECIMAL_HPP

#include <limits>

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Decimal : public Immediate {
public:
  explicit Decimal(std::string s, size_t tLine, size_t tColumn,
                   const std::string &tFile, bool isNegative = false)
      : Immediate(std::move(s), tLine, tColumn, tFile) {
    if (token.length() > 7) {
      // 7 digits (i.e. #-12345) is the largest that can fit inside a 16 bit
      // number
      tooBig = true;
      return;
    }
    switch (token.front()) {
    case '#': {
      char *check = nullptr;
      const auto v = std::strtoll(token.c_str() + 1ull, &check, 10);

      if (nullptr == check || v > std::numeric_limits<int16_t>::max() ||
          v < std::numeric_limits<int16_t>::min()) {
        tooBig = true;
      } else {
        mValue = static_cast<std::int16_t>(v);
      }
      break;
    }
    default: {
      char *check = nullptr;
      auto v = std::strtoll(token.c_str(), &check, 10);

      if (isNegative) {
        v = -v;
        token = "-" + token;
      }

      if (nullptr == check || v > std::numeric_limits<int16_t>::max() ||
          v < std::numeric_limits<int16_t>::min()) {
        tooBig = true;
      } else {
        mValue = static_cast<std::int16_t>(v);
      }
      break;
    }
    }
  } // namespace Token

  Decimal(const Decimal &) = default;
  Decimal(Decimal &&) noexcept = default;

  Decimal &operator=(const Decimal &) = default;
  Decimal &operator=(Decimal &&) noexcept = default;
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif
