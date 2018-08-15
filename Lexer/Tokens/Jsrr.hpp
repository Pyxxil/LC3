#ifndef TOKEN_JSRR_HPP
#define TOKEN_JSRR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jsrr : public Token {
public:
  explicit Jsrr(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::REGISTER)})) {}

  Jsrr(const Jsrr &) = default;
  Jsrr(Jsrr &&) noexcept = default;

  Jsrr &operator=(const Jsrr &) = default;
  Jsrr &operator=(Jsrr &&) noexcept = default;

  TokenType tokenType() const final { return JSRR; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif