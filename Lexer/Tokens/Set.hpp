#ifndef TOKEN_SET_HPP
#define TOKEN_SET_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Set : public Token
{
public:
  explicit Set(std::string t)
    : Token(std::move(t),
            Requirements(
              2,
              { Match(Token_Type::REGISTER),
                Match(Token_Type::IMMEDIATE) | Match(Token_Type::LABEL) }))
  {}

  Set(const Set&) = default;
  Set(Set&&) noexcept = default;

  Set& operator=(const Set&) = default;
  Set& operator=(Set&&) noexcept = default;

  ~Set() override = default;

  Token_Type tokenType() const final { return SET; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif