#ifndef TOKEN_HALT_HPP
#define TOKEN_HALT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Halt : public Token {
public:
  explicit Halt(std::string t) : Token(std::move(t)) {}

  Halt(const Halt &) = default;
  Halt(Halt &&) noexcept = default;

  Halt &operator=(const Halt &) = default;
  Halt &operator=(Halt &&) noexcept = default;

  ~Halt() override = default;

  Token_Type tokenType() const final { return HALT; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif