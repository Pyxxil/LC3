#ifndef TOKEN_BR_HPP
#define TOKEN_BR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Br : public Token {
public:
  Br(std::string t, bool n, bool z, bool p, size_t tLine, size_t tColumn,
     const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  1, {Match(TokenType::LABEL) | Match(TokenType::IMMEDIATE)})),
        N(n), Z(z), P(p) {
    (void)N;
    (void)Z;
    (void)P;
  }

  Br(const Br &) = default;
  Br(Br &&) noexcept = default;

  Br &operator=(const Br &) = default;
  Br &operator=(Br &&) noexcept = default;

  TokenType tokenType() const final { return BR; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override {
    
  }

  word memoryRequired() const override { return 1_word; }

private:
  bool N;
  bool Z;
  bool P;
};
} // namespace Token
} // namespace Lexer

#endif