#ifndef TOKEN_JMP_HPP
#define TOKEN_JMP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jmp : public Token {
public:
  explicit Jmp(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::REGISTER)})) {}

  Jmp(const Jmp &) = default;
  Jmp(Jmp &&) noexcept = default;

  Jmp &operator=(const Jmp &) = default;
  Jmp &operator=(Jmp &&) noexcept = default;

  TokenType tokenType() const final { return JMP; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif