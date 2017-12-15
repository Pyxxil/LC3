#ifndef TOKEN_JSR_HPP
#define TOKEN_JSR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jsr : public Token
{
public:
  explicit Jsr(std::string t)
    : Token(std::move(t), Requirements(1, { Match(Token_Type::LABEL) }))
  {}

  Jsr(const Jsr&) = default;
  Jsr(Jsr&&) noexcept = default;

  Jsr& operator=(const Jsr&) = default;
  Jsr& operator=(Jsr&&) noexcept = default;

  ~Jsr() override = default;

  Token_Type tokenType() const final { return JSR; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif