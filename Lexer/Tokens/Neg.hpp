#ifndef TOKEN_NEG_HPP
#define TOKEN_NEG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Neg : public Token
{
public:
  explicit Neg(std::string t)
    : Token(std::move(t),
            Requirements(1,
                         { Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::REGISTER) },
                         3))
  {}

  Neg(const Neg&) = default;
  Neg(Neg&&) noexcept = default;

  Neg& operator=(const Neg&) = default;
  Neg& operator=(Neg&&) noexcept = default;

  ~Neg() override = default;

  Token_Type tokenType() const final { return NEG; }

private:
};
}
}

#endif