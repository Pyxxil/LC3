#include "Decimal.hpp"

#include <limits>

namespace Lexer::Token {

Decimal::Decimal(std::string s, size_t t_line, size_t t_column,
                 const std::string &t_file, bool is_negative)
    : Immediate(std::move(s), t_line, t_column, t_file) {
  if (token.length() > 7) {
    // 7 digits (i.e. #-12345) is the largest that can fit inside a 16 bit
    // number
    too_big = true;
    return;
  }
  switch (token.front()) {
  case '#': {
    char *check = nullptr;
    const auto v = std::strtoll(token.c_str() + 1ull, &check, 10);

    if (nullptr == check || v > std::numeric_limits<int16_t>::max() ||
        v < std::numeric_limits<int16_t>::min()) {
      too_big = true;
    } else {
      m_value = static_cast<std::int16_t>(v);
    }
    break;
  }
  default: {
    char *check = nullptr;
    auto v = std::strtoll(token.c_str(), &check, 10);

    if (is_negative) {
      v = -v;
      token = "-" + token;
    }

    if (nullptr == check || v > std::numeric_limits<int16_t>::max() ||
        v < std::numeric_limits<int16_t>::min()) {
      too_big = true;
    } else {
      m_value = static_cast<std::int16_t>(v);
    }
    break;
  }
  }
}

} // namespace Lexer::Token