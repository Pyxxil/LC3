#ifndef TOKEN_TRAP_HPP
#define TOKEN_TRAP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Trap : public Token
{
public:
  explicit Trap(std::string t)
    : Token(std::move(t), Requirements(1, { Match(Token_Type::IMMEDIATE) }))
  {}

  Trap(const Trap&) = default;
  Trap(Trap&&) noexcept = default;

  Trap& operator=(const Trap&) = default;
  Trap& operator=(Trap&&) noexcept = default;

  ~Trap() override = default;

  Token_Type tokenType() const final { return TRAP; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif