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

private:
};
#endif
} // namespace Token
} // namespace Lexer

#endif
