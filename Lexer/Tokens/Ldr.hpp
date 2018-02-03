#ifndef TOKEN_LDR_HPP
#define TOKEN_LDR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ldr : public Token {
public:
  explicit Ldr(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(3, {Match(TokenType::REGISTER),
                               Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE)})) {}

  Ldr(const Ldr &) = default;
  Ldr(Ldr &&) noexcept = default;

  Ldr &operator=(const Ldr &) = default;
  Ldr &operator=(Ldr &&) noexcept = default;

  TokenType tokenType() const final { return LDR; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif