#ifndef TOKEN_PUTSP_HPP
#define TOKEN_PUTSP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Putsp : public Token {
public:
  Putsp(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Putsp(const Putsp &) = default;
  Putsp(Putsp &&) noexcept = default;

  Putsp &operator=(const Putsp &) = default;
  Putsp &operator=(Putsp &&) noexcept = default;

  TokenType tokenType() const final { return PUTSP; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif