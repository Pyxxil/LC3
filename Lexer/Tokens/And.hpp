#ifndef TOKEN_AND_HPP
#define TOKEN_AND_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class And : public Token
{
public:
  explicit And(std::string t)
    : Token(std::move(t),
            Requirements(3,
                         { Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::REGISTER) |
                             Token_Match(Token_Type::IMMEDIATE) }))
  {}

  And(const And&) = default;
  And(And&&) noexcept = default;

  And& operator=(const And&) = default;
  And& operator=(And&&) noexcept = default;

  ~And() override = default;

  Token_Type tokenType() const final { return AND; }

private:
};
}
}

#endif