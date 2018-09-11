#ifndef TOKEN_CHARACTER_HPP
#define TOKEN_CHARACTER_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Character : public Immediate {
public:
  Character(std::string s, size_t tLine, size_t tColumn,
            const std::string &tFile)
      : Immediate(std::move(s), tLine, tColumn, tFile) {
    DEBUG("Found character as '{}'", token);
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
      }
    }
    mValue = static_cast<int16_t>(token.front());
  }

  Character(const Character &) = default;
  Character(Character &&) noexcept = default;

  Character &operator=(const Character &) = default;
  Character &operator=(Character &&) noexcept = default;

  const std::string &getToken() const override {
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
