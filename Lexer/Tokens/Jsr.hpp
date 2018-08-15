#ifndef TOKEN_JSR_HPP
#define TOKEN_JSR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jsr : public Token {
public:
  explicit Jsr(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::LABEL) |
                               Match(TokenType::IMMEDIATE)})) {}

  Jsr(const Jsr &) = default;
  Jsr(Jsr &&) noexcept = default;

  Jsr &operator=(const Jsr &) = default;
  Jsr &operator=(Jsr &&) noexcept = default;

  TokenType tokenType() const final { return JSR; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif