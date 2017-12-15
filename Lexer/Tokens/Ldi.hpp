#ifndef TOKEN_LDI_HPP
#define TOKEN_LDI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ldi : public Token
{
public:
  explicit Ldi(std::string t)
    : Token(std::move(t),
            Requirements(
              2,
              { Match(Token_Type::REGISTER),
                Match(Token_Type::LABEL) | Match(Token_Type::IMMEDIATE) }))
  {}

  Ldi(const Ldi&) = default;
  Ldi(Ldi&&) noexcept = default;

  Ldi& operator=(const Ldi&) = default;
  Ldi& operator=(Ldi&&) noexcept = default;

  ~Ldi() override = default;

  Token_Type tokenType() const final { return LDI; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif