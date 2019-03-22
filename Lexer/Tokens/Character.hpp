#ifndef TOKEN_CHARACTER_HPP
#define TOKEN_CHARACTER_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Character : public Immediate {
public:
  Character(const std::string &s, size_t tLine, size_t tColumn,
            const std::string &tFile)
      : Immediate(std::move(s), tLine, tColumn, tFile) {
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
