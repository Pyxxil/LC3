#ifndef TOKEN_RET_HPP
#define TOKEN_RET_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ret : public Token
{
public:
  explicit Ret(std::string t)
    : Token(std::move(t), Requirements())
  {}

  Ret(const Ret&) = default;
  Ret(Ret&&) noexcept = default;

  Ret& operator=(const Ret&) = default;
  Ret& operator=(Ret&&) noexcept = default;

  ~Ret() override = default;

  Token_Type tokenType() const final { return RET; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif