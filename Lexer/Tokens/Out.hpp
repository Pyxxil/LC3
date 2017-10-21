#ifndef TOKEN_OUT_HPP
#define TOKEN_OUT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Out : public Token
{
public:
  explicit Out(std::string t)
    : Token(std::move(t))
  {}

  Out(const Out&) = default;
  Out(Out&&) noexcept = default;

  Out& operator=(const Out&) = default;
  Out& operator=(Out&&) noexcept = default;

  ~Out() override = default;

  Token_Type tokenType() const final { return OUT; }

private:
};
}
}

#endif