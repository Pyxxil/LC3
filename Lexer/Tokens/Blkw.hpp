#ifndef TOKEN_BLKW_HPP
#define TOKEN_BLKW_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Blkw : public Token
{
public:
  explicit Blkw(std::string t)
    : Token(std::move(t),
            Requirements(1, { Token_Match(Token_Type::IMMEDIATE) }))
  {}

  Blkw(const Blkw&) = default;
  Blkw(Blkw&&) noexcept = default;

  Blkw& operator=(const Blkw&) = default;
  Blkw& operator=(Blkw&&) noexcept = default;

  ~Blkw() override = default;

  Token_Type tokenType() const final { return BLKW; }

private:
};
}
}

#endif