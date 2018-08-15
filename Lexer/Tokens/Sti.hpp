#ifndef TOKEN_STI_HPP
#define TOKEN_STI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Sti : public Token {
public:
  explicit Sti(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(3, {Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE) |
                                   Match(TokenType::LABEL)})) {}

  Sti(const Sti &) = default;
  Sti(Sti &&) noexcept = default;

  Sti &operator=(const Sti &) = default;
  Sti &operator=(Sti &&) noexcept = default;

  TokenType tokenType() const final { return STI; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif