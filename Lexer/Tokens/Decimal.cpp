#include "Decimal.hpp"

#include <charconv>
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

  auto &&[_, err] =
      std::from_chars(token.c_str() + static_cast<int>('#' == token.front()),
                      token.c_str() + token.size(), m_value);
  too_big = err != std::errc();
}

} // namespace Lexer::Token