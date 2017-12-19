#ifndef TOKEN_LD_HPP
#define TOKEN_LD_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ld : public Token {
public:
  explicit Ld(std::string t)
      : Token(std::move(t),
              Requirements(2, {Match(Token_Type::REGISTER),
                               Match(Token_Type::LABEL) |
                                   Match(Token_Type::IMMEDIATE)})) {}

  Ld(const Ld &) = default;
  Ld(Ld &&) noexcept = default;

  Ld &operator=(const Ld &) = default;
  Ld &operator=(Ld &&) noexcept = default;

  ~Ld() override = default;

  Token_Type tokenType() const final { return LD; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif