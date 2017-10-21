#ifndef TOKEN_STR_HPP
#define TOKEN_STR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Str : public Token
{
public:
  explicit Str(std::string t)
    : Token(std::move(t),
            Requirements(3,
                         { Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::REGISTER),
                           Token_Match(Token_Type::IMMEDIATE) }))
  {}

  Str(const Str&) = default;
  Str(Str&&) noexcept = default;

  Str& operator=(const Str&) = default;
  Str& operator=(Str&&) noexcept = default;

  ~Str() override = default;

  Token_Type tokenType() const final { return STR; }

private:
};
}
}

#endif