#ifndef TOKEN_JMP_HPP
#define TOKEN_JMP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jmp : public Token
{
public:
  explicit Jmp(std::string t)
    : Token(std::move(t),
            Requirements(1, { Token_Match(Token_Type::REGISTER) }))
  {}

  Jmp(const Jmp&) = default;
  Jmp(Jmp&&) noexcept = default;

  Jmp& operator=(const Jmp&) = default;
  Jmp& operator=(Jmp&&) noexcept = default;

  ~Jmp() override = default;

  Token_Type tokenType() const final { return JMP; }

private:
};
}
}

#endif