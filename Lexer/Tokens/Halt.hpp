#ifndef TOKEN_HALT_HPP
#define TOKEN_HALT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Halt : public Token {
public:
  explicit Halt(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Halt(const Halt &) = default;
  Halt(Halt &&) noexcept = default;

  Halt &operator=(const Halt &) = default;
  Halt &operator=(Halt &&) noexcept = default;

  TokenType tokenType() const final { return HALT; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif