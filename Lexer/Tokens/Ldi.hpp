#ifndef TOKEN_LDI_HPP
#define TOKEN_LDI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ldi : public Token {
public:
  explicit Ldi(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::LABEL) |
                                   Match(TokenType::IMMEDIATE)})) {}

  Ldi(const Ldi &) = default;
  Ldi(Ldi &&) noexcept = default;

  Ldi &operator=(const Ldi &) = default;
  Ldi &operator=(Ldi &&) noexcept = default;

  TokenType tokenType() const final { return LDI; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif