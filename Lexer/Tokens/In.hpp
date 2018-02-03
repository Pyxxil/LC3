#ifndef TOKEN_IN_HPP
#define TOKEN_IN_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class In : public Token {
public:
  explicit In(std::string t, size_t tLine, size_t tColumn,
              const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  In(const In &) = default;
  In(In &&) noexcept = default;

  In &operator=(const In &) = default;
  In &operator=(In &&) noexcept = default;

  TokenType tokenType() const final { return IN; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif