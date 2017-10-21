#ifndef TOKEN_LEA_HPP
#define TOKEN_LEA_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Lea : public Token
{
public:
  explicit Lea(std::string t)
    : Token(std::move(t),
            Requirements(2,
                         { Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::LABEL) |
                             Token_Match(Token_Type::IMMEDIATE) }))
  {}

  Lea(const Lea&) = default;
  Lea(Lea&&) noexcept = default;

  Lea& operator=(const Lea&) = default;
  Lea& operator=(Lea&&) noexcept = default;

  ~Lea() override = default;

  Token_Type tokenType() const final { return LEA; }

private:
};
}
}

#endif