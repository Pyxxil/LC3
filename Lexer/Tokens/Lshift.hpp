#ifndef TOKEN_LSHIFT_HPP
#define TOKEN_LSHIFT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Lshift : public Token
{
public:
  explicit Lshift(std::string t)
    : Token(std::move(t),
            Requirements(
              2,
              { Match(Token_Type::REGISTER), Match(Token_Type::IMMEDIATE) }))
  {}

  Lshift(const Lshift&) = default;
  Lshift(Lshift&&) noexcept = default;

  Lshift& operator=(const Lshift&) = default;
  Lshift& operator=(Lshift&&) noexcept = default;

  ~Lshift() override = default;

  Token_Type tokenType() const final { return LSHIFT; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif