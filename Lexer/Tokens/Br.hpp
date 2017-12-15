#ifndef TOKEN_BR_HPP
#define TOKEN_BR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Br : public Token
{
public:
  Br(std::string t, bool n, bool z, bool p)
    : Token(std::move(t),
            Requirements(
              1,
              { Match(Token_Type::LABEL) | Match(Token_Type::IMMEDIATE) }))
    , N(n)
    , Z(z)
    , P(p)
  {
    (void)N;
    (void)Z;
    (void)P;
  }

  Br(const Br&) = default;
  Br(Br&&) noexcept = default;

  Br& operator=(const Br&) = default;
  Br& operator=(Br&&) noexcept = default;

  ~Br() override = default;

  Token_Type tokenType() const final { return BR; }

  void assemble() override { Token::assemble(); }

private:
  bool N;
  bool Z;
  bool P;
};
}
}

#endif