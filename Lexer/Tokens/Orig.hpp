#ifndef TOKEN_ORIG_HPP
#define TOKEN_ORIG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Orig : public Token
{
public:
  explicit Orig(std::string t)
    : Token(std::move(t), Requirements(1, { Match(Token_Type::IMMEDIATE) }))
  {}

  Orig(const Orig&) = default;
  Orig(Orig&&) noexcept = default;

  Orig& operator=(const Orig&) = default;
  Orig& operator=(Orig&&) noexcept = default;

  ~Orig() override = default;

  Token_Type tokenType() const final { return ORIG; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif