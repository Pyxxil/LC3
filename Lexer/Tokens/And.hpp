#ifndef TOKEN_AND_HPP
#define TOKEN_AND_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class And : public Token {
public:
  explicit And(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  2,
                  {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                   Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                  3)) {}

  And(const And &) = default;
  And(And &&) noexcept = default;

  And &operator=(const And &) = default;
  And &operator=(And &&) noexcept = default;

  TokenType tokenType() const final { return AND; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif