#ifndef TOKEN_CHARACTER_HPP
#define TOKEN_CHARACTER_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Character : public Immediate {
public:
  Character(std::string s, size_t t_line, size_t t_column,
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
      default: break;
      }
    }
    m_value = static_cast<int16_t>(token.front());
  }

  Character(const Character &) = default;
  Character(Character &&) = default;

  Character &operator=(const Character &) = default;
  Character &operator=(Character &&) = default;

  const std::string &get_token() const override {
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

private:
  mutable std::string character{};
};
#endif
} // namespace Token
} // namespace Lexer

#endif
