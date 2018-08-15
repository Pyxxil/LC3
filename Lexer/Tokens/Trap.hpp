#ifndef TOKEN_TRAP_HPP
#define TOKEN_TRAP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Trap : public Token {
public:
  Trap(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

  Trap(const Trap &) = default;
  Trap(Trap &&) noexcept = default;

  Trap &operator=(const Trap &) = default;
  Trap &operator=(Trap &&) noexcept = default;

  TokenType tokenType() const final { return TRAP; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif