#ifndef TOKEN_IN_HPP
#define TOKEN_IN_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class In : public Token
{
public:
  explicit In(std::string t)
    : Token(std::move(t))
  {}

  In(const In&) = default;
  In(In&&) noexcept = default;

  In& operator=(const In&) = default;
  In& operator=(In&&) noexcept = default;

  ~In() override = default;

  Token_Type tokenType() const final { return IN; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif