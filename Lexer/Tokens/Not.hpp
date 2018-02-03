#ifndef TOKEN_NOT_HPP
#define TOKEN_NOT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Not : public Token {
public:
  explicit Not(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                  2)) {}

  Not(const Not &) = default;
  Not(Not &&) noexcept = default;

  Not &operator=(const Not &) = default;
  Not &operator=(Not &&) noexcept = default;

  TokenType tokenType() const final { return NOT; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif