#ifndef TOKEN_JSRR_HPP
#define TOKEN_JSRR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jsrr : public Token
{
public:
  explicit Jsrr(std::string t)
    : Token(std::move(t),
            Requirements(1, { Token_Match(Token_Type::REGISTER) }))
  {}

  Jsrr(const Jsrr&) = default;
  Jsrr(Jsrr&&) noexcept = default;

  Jsrr& operator=(const Jsrr&) = default;
  Jsrr& operator=(Jsrr&&) noexcept = default;

  ~Jsrr() override = default;

  Token_Type tokenType() const final { return JSRR; }

private:
};
}
}

#endif