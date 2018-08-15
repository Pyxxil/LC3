#ifndef TOKEN_STR_HPP
#define TOKEN_STR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Str : public Token {
public:
  explicit Str(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(3, {Match(TokenType::REGISTER),
                               Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE)})) {}

  Str(const Str &) = default;
  Str(Str &&) noexcept = default;

  Str &operator=(const Str &) = default;
  Str &operator=(Str &&) noexcept = default;

  TokenType tokenType() const final { return STR; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif