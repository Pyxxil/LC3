#ifndef TOKEN_PUTS_HPP
#define TOKEN_PUTS_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Puts : public Token {
public:
  Puts(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Puts(const Puts &) = default;
  Puts(Puts &&) noexcept = default;

  Puts &operator=(const Puts &) = default;
  Puts &operator=(Puts &&) noexcept = default;

  TokenType tokenType() const final { return PUTS; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif