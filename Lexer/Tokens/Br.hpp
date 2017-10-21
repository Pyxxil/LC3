#ifndef TOKEN_BR_HPP
#define TOKEN_BR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Br : public Token
{
public:
  Br(std::string t, bool n, bool z, bool p)
    : Token(std::move(t), Requirements(1, { Token_Match(Token_Type::LABEL) }))
    , N(n)
    , Z(z)
    , P(p)
  {}

  Br(const Br&) = default;
  Br(Br&&) noexcept = default;

  Br& operator=(const Br&) = default;
  Br& operator=(Br&&) noexcept = default;

  ~Br() override = default;

  Token_Type tokenType() const final { return BR; }

private:
  bool N;
  bool Z;
  bool P;
};
}
}

#endif