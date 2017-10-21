#ifndef TOKEN_INCLUDE_HPP
#define TOKEN_INCLUDE_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Include : public Token
{
public:
  explicit Include(std::string t)
    : Token(std::move(t), Requirements(1, { Token_Match(Token_Type::STRING) }))
  {}

  Include(const Include&) = default;
  Include(Include&&) noexcept = default;

  Include& operator=(const Include&) = default;
  Include& operator=(Include&&) noexcept = default;

  ~Include() override = default;

  Token_Type tokenType() const final { return INCLUDE; }

private:
};
}
}

#endif