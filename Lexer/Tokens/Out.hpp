#ifndef TOKEN_OUT_HPP
#define TOKEN_OUT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Out : public Token {
public:
  Out(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Out(const Out &) = default;
  Out(Out &&) noexcept = default;

  Out &operator=(const Out &) = default;
  Out &operator=(Out &&) noexcept = default;

  TokenType tokenType() const final { return OUT; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif