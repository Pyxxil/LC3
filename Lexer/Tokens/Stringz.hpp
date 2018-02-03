#ifndef TOKEN_STRINGZ_HPP
#define TOKEN_STRINGZ_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Stringz : public Token {
public:
  Stringz(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::STRING)})) {}

  Stringz(const Stringz &) = default;
  Stringz(Stringz &&) noexcept = default;

  Stringz &operator=(const Stringz &) = default;
  Stringz &operator=(Stringz &&) noexcept = default;

  TokenType tokenType() const final { return STRINGZ; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override {
    return static_cast<word>(operands().size());
  }

private:
};
} // namespace Token
} // namespace Lexer

#endif