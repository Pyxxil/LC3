#include "Character.hpp"

namespace Lexer::Token {
#ifdef ADDONS
Character::Character(std::string s, size_t t_line, size_t t_column,
                     const std::string &t_file)
    : Immediate(std::move(s), t_line, t_column, t_file) {
  if ('\\' == token.front()) {
    switch (token.back()) {
    case 'n':
      token = "\n";
      break;
    case 't':
      token = '\t';
      break;
    case '\'':
      token = "'";
      break;
    default:
      break;
    }
  }
  m_value = static_cast<int16_t>(token.front());
}

const std::string &Character::get_token() const {
  if (character.empty()) {
    character.push_back('\'');
    switch (token.front()) {
    case '\0':
      character = "\\0";
      break;
    case '\n':
      character = "\\n";
      break;
    case '\t':
      character = "\\t";
      break;
    default:
      character.push_back(token.front());
      break;
    }
    character.push_back('\'');
  }
  return character;
}
#endif
} // namespace Lexer::Token