#ifndef TOKEN_LD_HPP
#define TOKEN_LD_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ld : public Token {
public:
  explicit Ld(std::string t, size_t tLine, size_t tColumn,
              const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::LABEL) |
                                   Match(TokenType::IMMEDIATE)})) {}

  Ld(const Ld &) = default;
  Ld(Ld &&) noexcept = default;

  Ld &operator=(const Ld &) = default;
  Ld &operator=(Ld &&) noexcept = default;

  TokenType tokenType() const final { return LD; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif