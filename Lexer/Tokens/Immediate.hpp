#ifndef TOKEN_IMMEDIATE_HPP
#define TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Immediate : public Token
{
public:
  explicit Immediate(std::string t)
    : Token(std::move(t))
  {}

  Immediate(const Immediate&) = default;
  Immediate(Immediate&&) noexcept = default;

  Immediate& operator=(const Immediate&) = default;
  Immediate& operator=(Immediate&&) noexcept = default;

  ~Immediate() override = default;

  Token_Type tokenType() const final { return IMMEDIATE; }

  void negate() final
  {
    token = "-" + token;
    value = static_cast<uint16_t>(-value);
  }

  void assemble() override { Token::assemble(); }

private:
  uint16_t value{};
};
}
}

#endif
