#ifndef TOKEN_STRINGZ_HPP
#define TOKEN_STRINGZ_HPP

#include <numeric>

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Stringz : public Token {
public:
  Stringz(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(STRING)}, -1u)) {}

  Stringz(const Stringz &) = default;
  Stringz(Stringz &&) noexcept = default;

  Stringz &operator=(const Stringz &) = default;
  Stringz &operator=(Stringz &&) noexcept = default;

  TokenType tokenType() const final { return STRINGZ; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override {
    return static_cast<word>(std::accumulate(
        operands().begin(), operands().end(), 0, [](int sum, auto &&str) {
          return sum + str->Token::getToken().length() + 1;
        }));
  }

private:
};
} // namespace Token
} // namespace Lexer

#endif