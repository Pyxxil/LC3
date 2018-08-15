#ifndef TOKEN_NEG_HPP
#define TOKEN_NEG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Neg : public Token {
public:
  Neg(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                  3)) {}

  Neg(const Neg &) = default;
  Neg(Neg &&) noexcept = default;

  Neg &operator=(const Neg &) = default;
  Neg &operator=(Neg &&) noexcept = default;

  TokenType tokenType() const final { return NEG; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 2_words; }
};
} // namespace Token
} // namespace Lexer

#endif