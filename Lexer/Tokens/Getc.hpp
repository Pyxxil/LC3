#ifndef TOKEN_GETC_HPP
#define TOKEN_GETC_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Getc : public Token {
public:
  explicit Getc(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile, Requirements()) {}

  Getc(const Getc &) = default;
  Getc(Getc &&) noexcept = default;

  Getc &operator=(const Getc &) = default;
  Getc &operator=(Getc &&) noexcept = default;

  TokenType tokenType() const final { return GETC; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif