#include "Binary.hpp"

#include <bitset>

namespace Lexer::Token {

Binary::Binary(std::string s, size_t t_line, size_t t_column,
               const std::string &t_file, bool is_negative)
    : Immediate(std::move(s), t_line, t_column, t_file) {
  std::string immediate = token;
  if (immediate.length() > 2 && 'B' == std::toupper(immediate[1])) {
    immediate.erase(0, 2);
  } else if ('B' == std::toupper(immediate.front())) {
    immediate.erase(0, 1);
  }

  if (immediate.length() > 16) {
    too_big = true;
    return;
  }

  m_value = is_negative
                ? -static_cast<int16_t>(std::bitset<16>(immediate).to_ulong())
                : static_cast<int16_t>(std::bitset<16>(immediate).to_ulong());
}

} // namespace Lexer::Token