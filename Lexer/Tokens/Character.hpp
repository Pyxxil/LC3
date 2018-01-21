#ifndef TOKEN_CHARACTER_HPP
#define TOKEN_CHARACTER_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Character : public Immediate {
public:
  explicit Character(std::string s) : Immediate(std::move(s)) {}

  Character(const Character &) = default;
  Character(Character &&) noexcept = default;

  Character &operator=(const Character &) = default;
  Character &operator=(Character &&) noexcept = default;

  ~Character() override = default;

  const std::string &get_token() const override {
    if (character.empty()) {
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
