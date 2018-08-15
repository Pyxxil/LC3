#ifndef TOKEN_LEA_HPP
#define TOKEN_LEA_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Lea : public Token {
public:
  explicit Lea(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::LABEL) |
                                   Match(TokenType::IMMEDIATE)})) {}

  Lea(const Lea &) = default;
  Lea(Lea &&) noexcept = default;

  Lea &operator=(const Lea &) = default;
  Lea &operator=(Lea &&) noexcept = default;

  TokenType tokenType() const final { return LEA; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif