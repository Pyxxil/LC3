#ifndef TOKEN_ST_HPP
#define TOKEN_ST_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class St : public Token {
public:
  explicit St(std::string t, size_t tLine, size_t tColumn,
              const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::LABEL) |
                                   Match(TokenType::IMMEDIATE)})) {}

  St(const St &) = default;
  St(St &&) noexcept = default;

  St &operator=(const St &) = default;
  St &operator=(St &&) noexcept = default;

  TokenType tokenType() const final { return ST; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif