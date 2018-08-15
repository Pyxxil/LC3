#ifndef TOKEN_BLKW_HPP
#define TOKEN_BLKW_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Blkw : public Token {
public:
  explicit Blkw(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  1,
                  {Match(TokenType::IMMEDIATE),
                   Match(TokenType::IMMEDIATE) | Match(TokenType::LABEL)},
                  2)) {}

  Blkw(const Blkw &) = default;
  Blkw(Blkw &&) noexcept = default;

  Blkw &operator=(const Blkw &) = default;
  Blkw &operator=(Blkw &&) noexcept = default;

  TokenType tokenType() const final { return BLKW; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }
};
} // namespace Token
} // namespace Lexer

#endif