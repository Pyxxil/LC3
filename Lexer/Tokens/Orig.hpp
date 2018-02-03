#ifndef TOKEN_ORIG_HPP
#define TOKEN_ORIG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Orig : public Token {
public:
  explicit Orig(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

  Orig(const Orig &) = default;
  Orig(Orig &&) noexcept = default;

  Orig &operator=(const Orig &) = default;
  Orig &operator=(Orig &&) noexcept = default;

  TokenType tokenType() const final { return ORIG; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif