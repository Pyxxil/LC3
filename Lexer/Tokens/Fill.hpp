#ifndef TOKEN_FILL_HPP
#define TOKEN_FILL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Fill : public Token {
public:
  explicit Fill(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::IMMEDIATE) |
                               Match(TokenType::LABEL)})) {}

  Fill(const Fill &) = default;
  Fill(Fill &&) noexcept = default;

  Fill &operator=(const Fill &) = default;
  Fill &operator=(Fill &&) noexcept = default;

  TokenType tokenType() const final { return FILL; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif