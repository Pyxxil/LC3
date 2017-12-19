#ifndef TOKEN_PUTSP_HPP
#define TOKEN_PUTSP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Putsp : public Token {
public:
  explicit Putsp(std::string t) : Token(std::move(t)) {}

  Putsp(const Putsp &) = default;
  Putsp(Putsp &&) noexcept = default;

  Putsp &operator=(const Putsp &) = default;
  Putsp &operator=(Putsp &&) noexcept = default;

  ~Putsp() override = default;

  Token_Type tokenType() const final { return PUTSP; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif