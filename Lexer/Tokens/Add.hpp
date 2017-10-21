#ifndef TOKEN_ADD_HPP
#define TOKEN_ADD_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Add : public Token
{
public:
  explicit Add(std::string t)
    : Token(std::move(t),
            Requirements(3,
                         { Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::REGISTER) |
                             Token_Match(Token_Type::IMMEDIATE) }))
  {}

  Add(const Add&) = default;
  Add(Add&&) noexcept = default;

  Add& operator=(const Add&) = default;
  Add& operator=(Add&&) noexcept = default;

  ~Add() override = default;

  Token_Type tokenType() const final { return ADD; }

private:
};
}
}

#endif