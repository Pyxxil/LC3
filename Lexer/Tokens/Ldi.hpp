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
            Requirements(2,
                         { Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::LABEL) |
                             Token_Match(Token_Type::IMMEDIATE) }))
  {}

  Ldi(const Ldi&) = default;
  Ldi(Ldi&&) noexcept = default;

  Ldi& operator=(const Ldi&) = default;
  Ldi& operator=(Ldi&&) noexcept = default;

  ~Ldi() override = default;

  Token_Type tokenType() const final { return LDI; }

private:
};
}
}

#endif