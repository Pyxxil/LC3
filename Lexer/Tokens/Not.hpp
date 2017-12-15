#ifndef TOKEN_NOT_HPP
#define TOKEN_NOT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Not : public Token
{
public:
  explicit Not(std::string t)
    : Token(std::move(t),
            Requirements(
              2,
              { Match(Token_Type::REGISTER), Match(Token_Type::REGISTER) }))
  {}

  Not(const Not&) = default;
  Not(Not&&) noexcept = default;

  Not& operator=(const Not&) = default;
  Not& operator=(Not&&) noexcept = default;

  ~Not() override = default;

  Token_Type tokenType() const final { return NOT; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif