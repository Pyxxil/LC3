#ifndef TOKEN_END_HPP
#define TOKEN_END_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class End : public Token
{
public:
  explicit End(std::string t)
    : Token(std::move(t))
  {}

  End(const End&) = default;
  End(End&&) noexcept = default;

  End& operator=(const End&) = default;
  End& operator=(End&&) noexcept = default;

  ~End() override = default;

  Token_Type tokenType() const final { return END; }

private:
};
}
}

#endif