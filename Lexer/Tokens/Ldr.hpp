#ifndef TOKEN_LDR_HPP
#define TOKEN_LDR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ldr : public Token
{
public:
  explicit Ldr(std::string t)
    : Token(std::move(t),
            Requirements(3,
                         { Match(Token_Type::REGISTER),
                           Match(Token_Type::REGISTER),
                           Match(Token_Type::IMMEDIATE) }))
  {}

  Ldr(const Ldr&) = default;
  Ldr(Ldr&&) noexcept = default;

  Ldr& operator=(const Ldr&) = default;
  Ldr& operator=(Ldr&&) noexcept = default;

  ~Ldr() override = default;

  Token_Type tokenType() const final { return LDR; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif