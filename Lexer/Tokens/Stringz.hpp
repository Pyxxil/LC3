#ifndef TOKEN_STRINGZ_HPP
#define TOKEN_STRINGZ_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Stringz : public Token {
public:
  explicit Stringz(std::string t)
      : Token(std::move(t), Requirements(1, {Match(Token_Type::STRING)})) {}

  Stringz(const Stringz &) = default;
  Stringz(Stringz &&) noexcept = default;

  Stringz &operator=(const Stringz &) = default;
  Stringz &operator=(Stringz &&) noexcept = default;

  ~Stringz() override = default;

  Token_Type tokenType() const final { return STRINGZ; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif