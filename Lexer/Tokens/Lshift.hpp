#ifndef TOKEN_LSHIFT_HPP
#define TOKEN_LSHIFT_HPP

#include "Token.hpp"

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Lshift : public Token {
public:
  Lshift(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE)})) {}

  Lshift(const Lshift &) = default;
  Lshift(Lshift &&) noexcept = default;

  Lshift &operator=(const Lshift &) = default;
  Lshift &operator=(Lshift &&) noexcept = default;

  TokenType tokenType() const final { return LSHIFT; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override {
    return static_cast<word>(
        static_cast<Immediate *>(operands().back().get())->value());
  }
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif