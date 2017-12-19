#ifndef TOKEN_SUB_HPP
#define TOKEN_SUB_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Sub : public Token {
public:
  explicit Sub(std::string t)
      : Token(std::move(t), Requirements(2,
                                         {Match(Token_Type::REGISTER),
                                          Match(Token_Type::REGISTER),
                                          Match(Token_Type::REGISTER)},
                                         3)) {}

  Sub(const Sub &) = default;
  Sub(Sub &&) noexcept = default;

  Sub &operator=(const Sub &) = default;
  Sub &operator=(Sub &&) noexcept = default;

  ~Sub() override = default;

  Token_Type tokenType() const final { return SUB; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif