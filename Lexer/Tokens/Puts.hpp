#ifndef TOKEN_PUTS_HPP
#define TOKEN_PUTS_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Puts : public Token {
public:
  explicit Puts(std::string t) : Token(std::move(t)) {}

  Puts(const Puts &) = default;
  Puts(Puts &&) noexcept = default;

  Puts &operator=(const Puts &) = default;
  Puts &operator=(Puts &&) noexcept = default;

  ~Puts() override = default;

  Token_Type tokenType() const final { return PUTS; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif