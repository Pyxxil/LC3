#ifndef TOKEN_LEA_HPP
#define TOKEN_LEA_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Lea : public Token {
public:
  explicit Lea(std::string t)
      : Token(std::move(t),
              Requirements(2, {Match(Token_Type::REGISTER),
                               Match(Token_Type::LABEL) |
                                   Match(Token_Type::IMMEDIATE)})) {}

  Lea(const Lea &) = default;
  Lea(Lea &&) noexcept = default;

  Lea &operator=(const Lea &) = default;
  Lea &operator=(Lea &&) noexcept = default;

  ~Lea() override = default;

  Token_Type tokenType() const final { return LEA; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif