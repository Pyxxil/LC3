#ifndef TOKEN_FILL_HPP
#define TOKEN_FILL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Fill : public Token
{
public:
  explicit Fill(std::string t)
    : Token(std::move(t), Requirements(1, { Match(Token_Type::IMMEDIATE) }))
  {}

  Fill(const Fill&) = default;
  Fill(Fill&&) noexcept = default;

  Fill& operator=(const Fill&) = default;
  Fill& operator=(Fill&&) noexcept = default;

  ~Fill() override = default;

  Token_Type tokenType() const final { return FILL; }

  void assemble() override { Token::assemble(); }

private:
};
}
}

#endif