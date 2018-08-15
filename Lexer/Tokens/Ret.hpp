#ifndef TOKEN_RET_HPP
#define TOKEN_RET_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ret : public Token {
public:
  Ret(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile, Requirements()) {}

  Ret(const Ret &) = default;
  Ret(Ret &&) noexcept = default;

  Ret &operator=(const Ret &) = default;
  Ret &operator=(Ret &&) noexcept = default;

  TokenType tokenType() const final { return RET; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif