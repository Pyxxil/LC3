#ifndef TOKEN_STI_HPP
#define TOKEN_STI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Sti : public Token
{
public:
  explicit Sti(std::string t)
    : Token(std::move(t),
            Requirements(
              3,
              { Match(Token_Type::REGISTER),
                Match(Token_Type::IMMEDIATE) | Match(Token_Type::LABEL) }))
  {}

  Sti(const Sti&) = default;
  Sti(Sti&&) noexcept = default;

  Sti& operator=(const Sti&) = default;
  Sti& operator=(Sti&&) noexcept = default;

  ~Sti() override = default;

  Token_Type tokenType() const final { return STI; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif