#ifndef TOKEN_ST_HPP
#define TOKEN_ST_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class St : public Token {
public:
  explicit St(std::string t)
      : Token(std::move(t),
              Requirements(2, {Match(Token_Type::REGISTER),
                               Match(Token_Type::LABEL) |
                                   Match(Token_Type::IMMEDIATE)})) {}

  St(const St &) = default;
  St(St &&) noexcept = default;

  St &operator=(const St &) = default;
  St &operator=(St &&) noexcept = default;

  ~St() override = default;

  Token_Type tokenType() const final { return ST; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif