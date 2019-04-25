#include "Hexadecimal.hpp"

#include <charconv>

namespace Lexer::Token {

Hexadecimal::Hexadecimal(std::string s, size_t t_line, size_t t_column,
                         const std::string &t_file, bool is_negative)
    : Immediate(std::move(s), t_line, t_column, t_file) {

  std::string immediate = token;
  if ('-' == immediate.front()) {
    immediate.erase(0, 1);
    is_negative = true;
  }

  if (immediate.length() > 2 && 'X' == std::toupper(immediate[1])) {
    immediate.erase(0, 2);
  } else if ('X' == std::toupper(immediate.front())) {
    immediate.erase(0, 1);
  }

  if ('-' == immediate.front()) {
    immediate.erase(0, 1);
    is_negative = true;
  }

  if (immediate.length() > 4) {
    too_big = true;
    return;
  }

  uint16_t val;
  auto &&[_, err] = std::from_chars(
      immediate.c_str(), immediate.c_str() + immediate.size(), val, 16);
  too_big = err != std::errc();
  m_value =
      is_negative ? -static_cast<int16_t>(val) : static_cast<int16_t>(val);
}

} // namespace Lexer::Token