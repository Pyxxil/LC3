#ifndef TOKEN_BINARY_HPP
#define TOKEN_BINARY_HPP

#include <bitset>

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Binary : public Immediate {
public:
  explicit Binary(std::string s, size_t tLine, size_t tColumn,
                  const std::string &tFile, bool isNegative = false)
      : Immediate(std::move(s), tLine, tColumn, tFile) {
    std::string immediate = token;
    if (immediate.length() > 2 && 'B' == std::toupper(immediate[1])) {
      immediate.erase(0, 2);
    } else if ('B' == std::toupper(immediate.front())) {
      immediate.erase(0, 1);
    }

    if (immediate.length() > 16) {
      tooBig = true;
    } else {
      mValue =
          isNegative
              ? -static_cast<int16_t>(std::bitset<16>(immediate).to_ulong())
              : static_cast<int16_t>(std::bitset<16>(immediate).to_ulong());
    }
  }

  Binary(const Binary &) = default;
  Binary(Binary &&) noexcept = default;

  Binary &operator=(const Binary &) = default;
  Binary &operator=(Binary &&) noexcept = default;
};
} // namespace Token
} // namespace Lexer

#endif
