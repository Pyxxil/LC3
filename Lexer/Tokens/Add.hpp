#ifndef TOKEN_ADD_HPP
#define TOKEN_ADD_HPP

#include "../Token.hpp"

namespace Lexer {
namespace Token {
class Add : public Token {
public:
  explicit Add(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  2,
                  {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                   Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                  3)) {}

  Add(const Add &) = default;
  Add(Add &&) noexcept = default;

  Add &operator=(const Add &) = default;
  Add &operator=(Add &&) noexcept = default;

  TokenType tokenType() const final { return ADD; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif